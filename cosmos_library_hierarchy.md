# COSMOS Core — Library Dependency Hierarchy & Refactoring Analysis

## 1. Kernel / Support Split

**Kernel** — no dependency on `get_cosmosresources()`; runs without any COSMOS-provided
resource tables (IERS EOP, WMM.COF, JPL ephemeris, DEM tiles, spacecraft model files).
Can run on a bare embedded target or any host without a COSMOS installation.

**Support** — requires COSMOS-provided resource tables to function correctly.
The dividing line is the six libraries that directly call `get_cosmosresources()`:
`ephemlib`, `geomag`, `demlib`, `convertlib`, `physicslib`, `physicsclass`.
Everything that depends on any of these is also support.

Note: libraries that call `get_cosmosnodes()` / `get_cosmosrealms()` (user-supplied
node/realm data) or `data_base_path()` (creates output on the fly) are still **kernel**
by this definition — they do not require pre-installed COSMOS resource tables.

---

## 2. Proposed Layer Hierarchy

The goal is a strict DAG where each layer only depends on layers below it.
Files currently violating that rule are flagged ⚠.
The ════ line marks the kernel/support boundary.

```
                                                        ● SUPPORT
Layer 9 │ MODULES         file_module  node_propagator_module
        │                 packethandler_module  websocket_module
────────┤
Layer 8 │ DEVICES         acq_a35 (support – depends on agentclass)
        │                 all other device/* = KERNEL
────────┤
Layer 7 │ AGENT FRAMEWORK agentclass (support)
        │                 command_queue  event  scheduler  task = KERNEL
────────┤
Layer 6 │ PHYSICS SIM     physicsclass ⚠  simulatorclass  controllib
────────┤
Layer 5 │ HIGH-LEVEL      convertlib ⚠  jsonlib ⚠  physicslib
        │                 enumlib  (beacon  packetcomm  packethandler  FileSender
        │                           UdpSender = KERNEL despite being at this layer)
        │
        │  ⚠ convertlib.cpp  → physics/physicsclass.h  (circular)
        │  ⚠ jsonlib.cpp     → device/cpu/devicecpu.h  (layering violation)
        │                    → device/disk/devicedisk.h
        │
 ═══════╪══════════════════════════════ KERNEL / SUPPORT BOUNDARY ═════════════
        │   get_cosmosresources() first called here:
        │   ephemlib (JPL)  ·  geomag (WMM)  ·  demlib (DEM)
        │
                                                        ● KERNEL
Layer 4 │ DOMAIN/ORBIT    channellib  transferclass  transferlib  ax25class
        │ FOUNDATIONS     envi  estimation_lib  objlib  timeutils
        │                 nrlmsise-00  nrlmsise-00_data (pure computation – no COSMOS file deps)
────────┤
Layer 3 │ JSON / NAMESPACE jsondef  jsonclass  jsonobject  jsonvalue  json11
        │                 jpleph*  physics/constants
        │                 (* jpleph reads a file but path is given by caller)
────────┤
Layer 2 │ DATA I/O        datalib  timelib  socketlib  logger  check
        │                 (timelib includes ephemlib.h but never calls resource-loading functions)
────────┤
Layer 1 │ CORE UTILITIES  elapsedtime  stringlib  timeutils
────────┤
Layer 0 │ PRIMITIVES      math/* (bytelib crclib mathlib matrix vector lsfit rotation bindings)
        │                 configCosmos  configCosmosKernel  cosmos-errno  cosmos-errclass
        │                 sliplib  print_utils  json11  jsonclass  jsonobject  jsonvalue
```

### Kernel libraries (complete list)
math/\*, configCosmos, configCosmosKernel, cosmos-errno, cosmos-errclass,
elapsedtime, stringlib, print_utils, timeutils, sliplib, ax25class,
json11, jsonclass, jsonobject, jsonvalue, jsondef,
datalib, timelib, socketlib, logger, check,
jpleph, objlib, demlib (⚠ *demlib calls get_cosmosresources — see note below*),
channellib, transferlib, transferclass, envi, estimation_lib,
beacon, packetcomm, packethandler, FileSender, UdpSender,
physics/constants, nrlmsise-00, nrlmsise-00\_data,
command\_queue, event, scheduler, task,
arduino\_lib, spp, devicecpu, devicedisk, bbFctns, cssl\_lib, gige\_lib,
gs232b\_lib, ic9100\_lib, kisslib, kisstnc\_lib, kpc9612p\_lib, mixwtnc\_lib,
pic\_lib, prkx2su\_class, prkx2su\_lib, ts2000\_lib, unixgpio, usrp\_lib,
i2c, netradio, serialclass

> **Note on demlib:** demlib.cpp calls `get_cosmosresources()` to locate DEM tile
> files. It is therefore support, not kernel, despite sitting at Layer 3. It is
> listed in the spreadsheet as support.

### Support libraries (complete list)
ephemlib, geomag, **demlib**, convertlib, physicslib, physicsclass,
simulatorclass, controllib, enumlib, jsonlib,
agentclass, acq\_a35,
file\_module, node\_propagator\_module, packethandler\_module, websocket\_module

---

## 2. Confirmed Circular Dependencies

### Tangle A — `timelib` ↔ `elapsedtime` (medium severity)

| File | Direction |
|------|-----------|
| `support/timelib.h` line 55 | `#include "support/elapsedtime.h"` |
| `support/elapsedtime.cpp` line 38 | `#include "support/timelib.h"` |

**Why it exists**: `timelib.h` bundles `ElapsedTime` into its public interface as a convenience.
`elapsedtime.cpp` calls `currentmjd()` (defined in timelib) in `getElapsedTimeSince()`.

**Fix**: Move `currentmjd()` (or a minimal version of it) into a thin
`timebase.h/.cpp` at Layer 1 that has no dependencies.  Both `elapsedtime`
and `timelib` can then include `timebase.h`.  Remove `#include "elapsedtime.h"`
from `timelib.h`; expose it only in `timelib.h` if desired via a forward
declaration.

---

### Tangle B — `convertlib` ↔ `physicsclass` (critical severity)

| File | Direction |
|------|-----------|
| `support/convertlib.cpp` line 37 | `#include "physics/physicsclass.h"` |
| `physics/physicsclass.h` line 35 | `#include "support/convertlib.h"` |

**Why it exists**: Some functions in `convertlib.cpp` (Gauss-Jackson
initialisation, LVLH helpers) need `physicsclass` data structures, while
`physicsclass` needs coordinate-conversion routines from `convertlib`.

**Fix — extract a bridge file**:
1. Create `physics/physicsconvert.cpp` (new file, Layer 5.5).
2. Move into it every function in `convertlib.cpp` that touches
   `physicsclass` types (`GaussJacksonPositionPropagator`, `locstruc` with
   physics fields, etc.).
3. `convertlib.cpp` loses the `#include "physics/physicsclass.h"` line and
   stays at Layer 3.
4. `physicsconvert.cpp` includes both `convertlib.h` and `physicsclass.h` —
   one-way dependency only, no cycle.

---

### Tangle C — `jsonlib` → device libraries (critical severity — layering violation)

| File | Direction |
|------|-----------|
| `support/jsonlib.cpp` lines 40–41 | `#include "device/cpu/devicecpu.h"` |
|                                   | `#include "device/disk/devicedisk.h"` |

**Why it exists**: `jsonlib` serialises/deserialises device-state structs
(CPU, disk) as part of the COSMOS namespace.  Those structs are defined in
device headers.

**Fix — split jsonlib into two files**:
1. `support/jsonlib.cpp` — remove the device includes; keep all
   non-device serialisation.  Stays at Layer 3.
2. `support/jsonlib_hw.cpp` (new file, Layer 5) — contains only the
   functions that need `devicecpu.h` and `devicedisk.h`.  Includes both
   `jsonlib.h` and the device headers.
   Alternatively, move this code into `device/cpu/devicecpu_json.cpp` and
   `device/disk/devicedisk_json.cpp` so the device layer "registers itself"
   with the JSON namespace, keeping jsonlib completely unaware of devices.

---

## 3. Additional Simplification Opportunities

### 3a — Merge `timelib` and `elapsedtime` into `time/`

Both files are tightly coupled anyway.  A single `libraries/time/` module
with `timelib.h`, `elapsedtime.h`, and `timebase.h` would be cleaner than
two files scattered across `libraries/support/` that must include each other.

### 3b — `enumlib` depends on `convertlib` (surprising for a low-level utility)

`enumlib.cpp` line 34 includes `convertlib.h` in order to get coordinate
enum definitions.  If those enums were moved into `jsondef.h` or a new
`convertdef.h` they could be shared without enumlib needing all of convertlib.

### 3c — `nrlmsise-00` is currently at Layer 3 but is a standalone atmospheric model

It has no COSMOS dependencies; only C standard headers.  It could be moved
entirely into `libraries/math/` or a new `libraries/atmos/` directory to
reflect that it is a self-contained numerical model, not a COSMOS support
library.

### 3d — `convert_test_gui/` is not a library

The six `.cpp` files under `libraries/support/convert_test_gui/` are a Qt
GUI test application, not a library.  They should be moved to
`programs/tests/convert_test_gui/` to remove them from the library build.

### 3e — `module/` depends on both `agent` and `support` but is not optional infrastructure

The modules (`file_module`, `websocket_module`, etc.) are currently in
`libraries/module/` but each wraps a single agent capability.  Consider
moving them into `libraries/agent/modules/` to make their relationship with
the agent layer explicit.

---

## 4. Recommended Refactoring Priority

| Priority | Change | Effort | Benefit |
|----------|--------|--------|---------|
| 1 | Extract `physics/physicsconvert.cpp` to break convertlib ↔ physicsclass cycle | Medium | Eliminates critical circular dep; allows convertlib to be used without physics |
| 2 | Split `jsonlib.cpp` → `jsonlib.cpp` + `jsonlib_hw.cpp` | Small | Eliminates Layer 2→8 violation; makes jsonlib embeddable on targets without CPU/disk libs |
| 3 | Introduce `timebase.h` to decouple elapsedtime ↔ timelib | Small | Clarifies layer boundary; elapsedtime becomes truly standalone |
| 4 | Move `convert_test_gui/` out of libraries/ | Trivial | Cleans library structure |
| 5 | Move enums from enumlib→convertlib dependency into `jsondef.h` | Small | Allows enumlib to drop to Layer 1 |
| 6 | Move `nrlmsise-00` to `libraries/math/` or `libraries/atmos/` | Trivial | Better reflects its nature as a standalone model |
| 7 | Rename/relocate `module/` to `agent/modules/` | Trivial | Makes ownership clearer |

---

## 5. Dependency Matrix Summary

Cells show ✓ (allowed) or ⚠ (violation) for "row depends on column":

```
              math  support-L0  support-L1  support-L2  support-L3+  physics  agent  device  module
math           —      —           —           —           —            —        —      —       —
support-L0     ✓      —           —           —           —            —        —      —       —
support-L1     ✓      ✓           —           —           —            —        —      —       —
support-L2     ✓      ✓           ✓           —           —            —        —      —       —
support-L3+    ✓      ✓           ✓           ✓           —            —        —      —       —
convertlib     ✓      ✓           ✓           ✓           ✓            ⚠        —      —       —
jsonlib        ✓      ✓           ✓           ✓           ✓            —        —      ⚠       —
physicslib     ✓      ✓           ✓           ✓           ✓            —        —      —       —
physicsclass   ✓      ✓           ✓           ✓           ⚠            ✓        —      —       —
agent          ✓      ✓           ✓           ✓           ✓            ✓        —      —       —
device         ✓      ✓           ✓           ✓           ✓            ✓        ✓      —       —
module         ✓      ✓           ✓           ✓           ✓            ✓        ✓      ✓       —
```
