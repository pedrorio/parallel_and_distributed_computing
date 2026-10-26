# Proposal

## Why

The root README is three checklist lines. A reader who stops there never sees the matrix-factorisation recommender, the four solvers, or the speedups already measured in `compare/data`.

## What Changes

- Rewrite `README.md` with the problem, one line per variant, one speedup plot and a caption that uses only numbers in `compare/data`, build and run commands, and links to the variant write-ups and reports.
- Keep `- [x] MPI`. Serial and OpenMP stay checked.
- Leave `mpi/` and `mpi-hybrid/` as separate trees. Leave `.claude/` and `openspec/` in place.

## Capabilities

### New Capabilities

None.

### Modified Capabilities

None. `benchmark-reporting` and `solver-variants` already govern numeric claims and the split between the two MPI trees. Those requirements stay as they are. `.openspec.yaml` sets `skip_specs: true`.

## Impact

`README.md` and this change's planning files. Solver sources, comparison data, and plots stay as they are.
