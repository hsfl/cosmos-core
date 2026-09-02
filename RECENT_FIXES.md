# cosmos-core Recent Fixes

## Orbit Decay Bug — FIXED (2026-08-26)

**Branch:** ericdev — commit `01683b6f`

### Symptom

Mother satellite (HEX65W80H, 500 km 107° retrograde orbit, sttr realm) spiraled into Earth
by t≈2800s in both propagatorv3 and SMART SimRun. Reproducible with mother node alone.

### Root cause

`att.icrf.utc` is stuck at its initial value (~t=4s) in propagatorv3 because attitude is
never propagated. Inside `pos_eci2geoc` (convertlib.cpp), after correctly calling
`pos_extra(eci.utc)` to set up the j2e/e2j rotation matrices, `att_icrf2geoc` is called.
Internally, `att_icrf2geoc` calls `pos_extra(att.icrf.utc)` with the stale UTC — this
overwrites `loc.pos.extra` with matrices computed for the wrong epoch. By t=2800s the Earth
has rotated ~11.6° since t=4s, misdirecting gravity by ~1.7 m/s² → orbit collapse.

### Fix — `libraries/support/convertlib.cpp`, inside `pos_eci2geoc`

```cpp
// att_icrf2geoc calls pos_extra(att.icrf.utc) internally. If att.icrf.utc is stale
// (e.g. attitude not propagated in propagatorv3), that call overwrites extra with
// matrices for the wrong epoch, corrupting the j2e used for gravity rotation.
// Save and restore extra so the position-epoch matrices survive.
{
    extrapos saved_extra = loc.pos.extra;
    iretn = att_icrf2geoc(loc);
    loc.pos.extra = saved_extra;
}
```

### Also fixed — Moon gravity sign errors in `libraries/physics/physicsclass.cpp`

Two sign errors in `PosAccel` corrected:
- `ctpos` direction: `rv_sub(bodypos.s, eci.s)` → `rv_sub(rv_smult(-1., bodypos.s), eci.s)`
  (sat→Moon vector was pointing the wrong way)
- Frame correction: `rv_sub(..., da.to_rv())` → `rv_add(..., da.to_rv())`
  (Earth's Moon-induced acceleration must be added, not subtracted)

These errors cancel to first order (tidal force is correct to ~0.03%) so they were not the
decay root cause, but they are now analytically correct.

### Also fixed — GJ::Update() attitude sync in `libraries/physics/physicsclass.cpp`

Added `step[order2].loc.att.icrf = currentinfo->node.loc.att.icrf` before `PosAccel` in
`GaussJacksonPositionPropagator::Update()`. Prevents zero-quaternion NaN when GJ is
initialized before the attitude propagator (att.icrf.s=(0,0,0,0) → normalize() → NaN).

### Also fixed — `pos_extra` cache upgraded to `thread_local` in `libraries/support/convertlib.cpp`

Changed the cross-node result cache from `static` to `thread_local` to prevent torn reads
when SimRun (background thread) and Tick() (Qt thread) call `pos_extra` concurrently.

### Verification

3500s run with Earth+Sun+Moon forces: altitude oscillates 499–506 km (natural eccentricity),
no spiral. Previously crashed to −6 km at t=2800s.

### Propagated to

- `cosmosv5-agent` main: commit `62b0fea` (convertlib.cpp + physicsclass.cpp)
