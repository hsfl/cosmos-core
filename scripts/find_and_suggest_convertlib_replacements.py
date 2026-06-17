#!/usr/bin/env python3
"""
Scan a COSMOS core tree for type-1 (initiating) pos_xxx() and att_xxx() call
sites that should be migrated to pos_set_xxx() / att_set_xxx().

BACKGROUND
----------
Both pos_xxx() and att_xxx() serve two roles:

  Type 1 - Initiating propagation (MIGRATE THESE):
    The caller writes a new state into loc.pos.<frame> (or loc.att.<frame>),
    bumps that frame's .pass so it exceeds all others, then calls pos_xxx()
    (or att_xxx()) to derive every other frame.

        loc.pos.eci.pass++;
        pos_eci(loc);                  -> pos_set_eci(loc);

        loc.att.icrf.pass++;
        att_icrf(loc);                 -> att_set_icrf(loc);

  Type 2 - Cycle-breaking during recursive propagation (DO NOT TOUCH):
    Inside a pos_xxx() or att_xxx() body, after a frame-to-frame converter
    copies source.pass into the destination, the function calls pos_yyy() /
    att_yyy() recursively.  No pass is bumped externally.  These must never
    be replaced.

FILTER
------
Only files OUTSIDE convertlib.cpp / convertlib.h are reported.
Every call inside those files is type-2 by definition.

A .pass bump must appear within LOOKAHEAD lines before the pos/att call.

MIGRATION
---------
pos_set_xxx(loc) saves loc.pos.<frame>, calls pos_clear() (which also calls
att_clear()), restores the saved frame with pass=1, and calls pos_xxx(loc).
So:
  Old:                              New:
    loc.pos.eci.utc = t;              loc.pos.eci.utc = t;
    loc.pos.eci.s   = pos;            loc.pos.eci.s   = pos;
    loc.pos.eci.v   = vel;            loc.pos.eci.v   = vel;
    loc.pos.eci.a   = acc;            loc.pos.eci.a   = acc;
    loc.pos.eci.pass++;               pos_set_eci(loc);
    pos_eci(loc);

att_set_xxx(loc) saves loc.att.<frame>, calls att_clear(), restores with
pass=1, and calls att_xxx(loc).  Position is NOT cleared.
Prerequisite: pos_set_xxx() must have been called first for the same epoch.

Usage: python3 find_and_suggest_replacements.py /path/to/cosmos/core
"""

import re
import sys
import os

FRAMES = ["icrf", "eci", "sci", "geoc", "selc", "selg", "geos", "geod"]
FRAME_RE = "|".join(FRAMES)

ATT_FRAMES = ["icrf", "geoc", "selc"]   # frames with att_set_xxx defined
ATT_FRAME_RE = "|".join(ATT_FRAMES)

EXCLUDED_FILENAMES = {"convertlib.cpp", "convertlib.h"}

# OBJ: identifier chain (a.b, a->b, a->b.c, etc.)
OBJ = r"[A-Za-z_][A-Za-z0-9_]*(?:(?:->|\.)[A-Za-z_][A-Za-z0-9_]*)*(?:\[[^\]]*\])?"
ACCESS = r"(?:->|\.)"

def make_incr_re(struct, frame_re):
    return re.compile(
        rf"(?P<obj1>{OBJ}){ACCESS}{struct}\.(?P<frame1>{frame_re})\.pass\s*(\+\+|--)"
        rf"|(\+\+|--)\s*(?P<obj2>{OBJ}){ACCESS}{struct}\.(?P<frame2>{frame_re})\.pass"
        rf"|(?P<obj3>{OBJ}){ACCESS}{struct}\.(?P<frame3>{frame_re})\.pass\s*(\+=|-=)\s*1\b"
        rf"|(?P<obj4>{OBJ}){ACCESS}{struct}\.(?P<frame4>{frame_re})\.pass\s*=\s*(?!0\b)[0-9]+"
    )

POS_INCR_RE = make_incr_re("pos", FRAME_RE)
ATT_INCR_RE = make_incr_re("att", ATT_FRAME_RE)

def extract_obj_frame(m):
    for i in ("1", "2", "3", "4"):
        obj = m.group(f"obj{i}")
        frame = m.group(f"frame{i}")
        if obj and frame:
            return obj, frame
    return None, None

def make_call_re(prefix, frame, obj):
    return re.compile(
        rf"\b{prefix}_{re.escape(frame)}\s*\(\s*[*&]?\s*{re.escape(obj)}\s*\)"
    )

def scan_file(path, lookahead=6):
    try:
        with open(path, errors="replace") as f:
            lines = f.readlines()
    except OSError:
        return []

    results = []

    checks = [
        ("pos", POS_INCR_RE, "pos_set"),
        ("att", ATT_INCR_RE, "att_set"),
    ]

    for struct, incr_re, set_prefix in checks:
        propagator_prefix = struct  # pos_eci / att_icrf
        for i, line in enumerate(lines):
            m = incr_re.search(line)
            if not m:
                continue
            obj, frame = extract_obj_frame(m)
            if not obj or not frame:
                continue

            call_re = make_call_re(propagator_prefix, frame, obj)
            window_end = min(i + 1 + lookahead, len(lines))
            for j in range(i, window_end):
                if call_re.search(lines[j]):
                    results.append({
                        "struct":     struct,
                        "set_prefix": set_prefix,
                        "file":       path,
                        "incr_line":  i + 1,
                        "call_line":  j + 1,
                        "obj":        obj,
                        "frame":      frame,
                        "context":    [l.rstrip() for l in lines[max(0, i-4):j+2]],
                    })
                    break
    return results

def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    total = 0

    for dirpath, _, filenames in os.walk(root):
        for fn in sorted(filenames):
            if fn in EXCLUDED_FILENAMES:
                continue
            if not fn.endswith((".cpp", ".c", ".h", ".hpp")):
                continue

            path = os.path.join(dirpath, fn)
            for hit in scan_file(path):
                total += 1
                rel        = os.path.relpath(hit["file"], root)
                frame      = hit["frame"]
                obj        = hit["obj"]
                set_prefix = hit["set_prefix"]
                struct     = hit["struct"]

                print(f"\n{'='*72}")
                print(f"FILE  {rel}  lines {hit['incr_line']}–{hit['call_line']}  [{struct}.{frame}]")
                print(f"{'='*72}")
                print("Context:")
                ctx_start = max(1, hit["incr_line"] - 4)
                for ln, text in enumerate(hit["context"], start=ctx_start):
                    marker = ">>>" if ln in (hit["incr_line"], hit["call_line"]) else "   "
                    print(f"  {marker} {ln:4d}  {text}")
                print()
                print(f"  Replacement:")
                print(f"    {set_prefix}_{frame}({obj});")
                print(f"  (delete the pass bump and the {struct}_{frame}() call above)")
                if struct == "att":
                    print(f"  NOTE: pos_set_xxx(loc) must have been called for the same epoch")
                    print(f"        before this att_set_{frame}() call.")

    print(f"\n{'='*72}")
    print(f"Total candidate sites: {total}")
    if total > 0:
        print(f"(All outside convertlib.cpp/convertlib.h --")
        print(f" internal recursive calls are type-2 and left untouched.)")

if __name__ == "__main__":
    main()
