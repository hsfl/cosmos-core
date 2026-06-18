# COSMOS Core — Library Dependency Hierarchy

## 1. Kernel / Support Split

**Kernel** — no dependency on `get_cosmosresources()`; no disk I/O for
resource tables; no planetary conversion logic. Can run on a bare embedded
target or any host without a COSMOS installation.

**Support** — requires COSMOS-provided resource files (IERS EOP, WMM.COF,
JPL ephemeris, DEM tiles, spacecraft model files), or performs planetary /
coordinate conversions that are meaningless without them, or reads from disk
as part of normal operation.

Libraries that call `get_cosmosnodes()` / `get_cosmosrealms()` or
`data_base_path()` (creates output on the fly) are still **kernel**.

Kernel and support each have independent layer numbering starting at 0.

---

## 2. Layer Hierarchy

```
                                                        ● SUPPORT
Support Layer 4 │ MODULES         file_module  node_propagator_module
                │                 packethandler_module  websocket_module
────────────────┤
Support Layer 3 │ AGENT UTILS     beacon  packethandler  command_queue
                │                 event  scheduler
                │                 acq_a35 (device – depends on agentclass)
────────────────┤
Support Layer 2 │ AGENT FRAMEWORK agentclass
────────────────┤
Support Layer 1 │ PHYSICS SIM     physicsclass  simulatorclass  controllib
────────────────┤
Support Layer 0 │ HIGH-LEVEL      convertlib  jsonlib  physicslib
                │                 ephemlib  geomag  demlib
                │                 jpleph  nrlmsise-00  nrlmsise-00_data
                │                 enumlib
 ═══════════════╪═══════════════ KERNEL / SUPPORT BOUNDARY ════════════════
Kernel Layer 5  │ MESSAGING       packetcomm  FileSender  UdpSender  task
                │ DEVICES         arduino_lib  spp  devicecpu  devicedisk
                │                 bbFctns  cssl_lib  gige_lib  gs232b_lib
                │                 ic9100_lib  kisslib  kisstnc_lib
                │                 kpc9612p_lib  mixwtnc_lib  pic_lib
                │                 prkx2su_class  prkx2su_lib  ts2000_lib
                │                 unixgpio  usrp_lib  i2c  netradio  serialclass
────────────────┤
Kernel Layer 4  │ DOMAIN/ORBIT    channellib  transferclass  transferlib
                │ FOUNDATIONS     envi  estimation_lib
────────────────┤
Kernel Layer 3  │ JSON/NAMESPACE  jsondef  objlib  physics/constants
────────────────┤
Kernel Layer 2  │ DATA I/O        datalib  socketlib  logger  check  ax25class
────────────────┤
Kernel Layer 1  │ CORE UTILITIES  elapsedtime  timelib  stringlib  timeutils
────────────────┤
Kernel Layer 0  │ PRIMITIVES      math/* (bytelib crclib mathlib matrix
                │                         vector lsfit rotation bindings)
                │                 configCosmos  configCosmosKernel
                │                 cosmos-errno  cosmos-errclass
                │                 sliplib  print_utils
                │                 json11  jsonclass  jsonobject  jsonvalue
                                                        ● KERNEL
```

### Notes on classifications

- **timelib** moved to K1: after cleanup, its only COSMOS deps are
  `configCosmos.h` (K0), `timeutils.h` (K1), and `stringlib.h` (K1 impl only).
- **elapsedtime** at K1: `elapsedtime.cpp` depends on `timelib.h` for
  `currentmjd()` — a clean same-layer dependency.
- **timed_countdown** moved from `timelib` to `elapsedtime` (it uses
  `ElapsedTime`); breaks the former circular `.cpp` dependency.
- **beacon**, **packethandler**, **command_queue**, **event**, **scheduler**
  reclassified as support S3 — all include `agentclass.h` or `jsonlib.h`.
- **ephemlib**, **geomag**, **demlib**, **jpleph**, **nrlmsise-00/data**,
  **convertlib** — disk-dependent or physics-simulation-only → support S0.

---

### Kernel libraries (complete list)

**K0 — Primitives:**
math/\* (bytelib, crclib, mathlib, matrix, vector, lsfit, rotation, bindings),
configCosmos, configCosmosKernel, cosmos-errno, cosmos-errclass,
sliplib, print_utils, json11, jsonclass, jsonobject, jsonvalue

**K1 — Core utilities:**
elapsedtime, timelib, stringlib, timeutils

**K2 — Data I/O:**
datalib, socketlib, logger, check, ax25class

**K3 — JSON / namespace:**
jsondef, objlib, physics/constants

**K4 — Domain / orbit foundations:**
channellib, transferclass, transferlib, envi, estimation_lib

**K5 — Messaging & devices:**
packetcomm, FileSender, UdpSender, task,
arduino\_lib, spp, devicecpu, devicedisk, bbFctns, cssl\_lib, gige\_lib,
gs232b\_lib, ic9100\_lib, kisslib, kisstnc\_lib, kpc9612p\_lib, mixwtnc\_lib,
pic\_lib, prkx2su\_class, prkx2su\_lib, ts2000\_lib, unixgpio, usrp\_lib,
i2c, netradio, serialclass

---

### Support libraries (complete list)

**S0 — High-level foundations:**
convertlib, jsonlib, physicslib,
ephemlib, geomag, demlib, jpleph, nrlmsise-00, nrlmsise-00\_data, enumlib

**S1 — Physics simulation:**
physicsclass, simulatorclass, controllib

**S2 — Agent framework:**
agentclass

**S3 — Agent utilities & devices:**
beacon, packethandler, command\_queue, event, scheduler, acq\_a35

**S4 — Modules:**
file\_module, node\_propagator\_module, packethandler\_module, websocket\_module

---

## 3. Resolved Issues

### Tangle A — `timelib` ↔ `elapsedtime` ✅ RESOLVED

**Root cause:** `elapsedtime.cpp` called `currentmjd()` from `timelib.h`;
`timelib.cpp` used `ElapsedTime` in `timed_countdown()`.

**Resolution:**
- `timed_countdown` moved from `timelib` to `elapsedtime` (it uses
  `ElapsedTime` — belongs there). Declaration moved to `elapsedtime.h`.
- `timelib.cpp` no longer includes `elapsedtime.h`.
- `elapsedtime.cpp` includes `timelib.h` — clean one-way dependency.
- `timelib` promoted to K1: only COSMOS deps are K0/K1.
- `timebase.h` (intermediate shim) was considered but not needed.

---

### Tangle B — `convertlib` ↔ `physicsclass` ✅ RESOLVED

Dead `#include "physics/physicsclass.h"` in `convertlib.cpp` removed (no
`Physics::` types were actually referenced). Dependency is now one-way:
physicsclass → convertlib. `convertlib` reclassified as support S0.

---

### Tangle C — `jsonlib` → device libraries ✅ RESOLVED

`DeviceCpu`/`DeviceDisk` OS-query classes were used only for hostname lookup
in three functions; replaced with POSIX `gethostname()`. Dead `json_create_cpu`
(no callers) removed. Stale `#include "support/ephemlib.h"` in `timelib.cpp`
also removed.

---

## 4. Open Simplification Opportunities

| # | Change | Effort |
|---|--------|--------|
| 1 | Move `convert_test_gui/` out of `libraries/` to `programs/tests/` | Trivial |
| 2 | Move enums from `enumlib`→`convertlib` dep into `jsondef.h` | Small |
| 3 | Rename/relocate `module/` to `agent/modules/` | Trivial |

---

## 5. Dependency Matrix

All previously flagged violations are resolved.

```
         K0  K1  K2  K3  K4  K5  S0  S1  S2  S3  S4
K0        —
K1        ✓   —
K2        ✓   ✓   —
K3        ✓   ✓   ✓   —
K4        ✓   ✓   ✓   ✓   —
K5        ✓   ✓   ✓   ✓   ✓   —
S0        ✓   ✓   ✓   ✓   ✓   —   —
S1        ✓   ✓   ✓   ✓   ✓   —   ✓   —
S2        ✓   ✓   ✓   ✓   ✓   —   ✓   ✓   —
S3        ✓   ✓   ✓   ✓   ✓   —   ✓   ✓   ✓   —
S4        ✓   ✓   ✓   ✓   ✓   —   ✓   ✓   ✓   ✓   —
```
