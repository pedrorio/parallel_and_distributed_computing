# Proposal

## Why

The 2D-grid hybrid MPI+OpenMP solver is already on `master` — it landed in June via
merged pull requests #2 and #3. What did not land is the evidence for it. The committed
benchmark data and plots were measured *before* the OpenMP optimisation and *before* the
MPI rewrite, so the published numbers describe code that no longer exists; three
documented claims about the implementation are inaccurate or incomplete; and the commit
adding CI never reached `master`.

The result is a repository whose code is in better condition than anything that
corroborates it: three sources of truth — code, data, reports — that disagree pairwise,
and no automation to catch it happening again.

## What Changes

- **Establish a verified baseline on `master`.** Build all three implementations and run
  the MPI CTest suite (correctness at np 1/2/4, non-square grids, empty blocks, hybrid
  path with pinned `OMP_NUM_THREADS`). The suite has been on `master` since June and has
  never been run on this machine, so "it works" is currently unverified here.
- **Land the CI workflows.** `.github/workflows/{tests,benchmark}.yml` exist only in
  commit `a1667ff`, which is not on `master`. Take the two workflow files only: the same
  commit also flips `README.md` from `- [x] MPI` back to `- [ ] MPI`, which would regress
  a line `master` already has correct.
- **Make `resultsAreCorrect` mean something.** `compare/scripts/benchmark.py` emits a
  hardcoded `0` for that column and runs every binary with `stdout=DEVNULL`, so
  correctness cannot be checked there even in principle. It should compare output against
  the instance's `.out` file, whose path it already has.
- **Regenerate the performance data** at `PARALLELISM="1 2 4 8"`, replacing
  `compare/data/comparison.{serial,omp,mpi}.csv` and the four plots, so the published
  numbers describe the shipped code. The committed CSVs measure the *replicate-all* MPI
  design and the *pre-optimisation* OpenMP, and they predate `benchmark.py` itself.
- **Decide the phase-breakdown question.** The script writes `0, 0, total, 0` for the four
  phase columns, flattening a breakdown that report prose still refers to. Either restore
  per-phase instrumentation or stop making per-phase claims.
- **Reconcile the reports with the regenerated data.** `mpi/report/main.tex` carries its
  grid-vs-replicate comparison as numbers hardcoded in LaTeX that never passed through the
  data pipeline; `omp/report/main.tex` cites `instML1M`, `instML100k`,
  `inst50000-5000-100-2-5`, `inst400-50000-30-200-500`, and `inst600-10000-10-40-400` —
  none of which exist in `instances/`. Both datasets stay, labelled by machine.
- **Correct the memory claim in `mpi/README.md`**, which headlines `(U*F + F*I)/sqrt(P)`
  but omits `dL`/`dR` (a second copy), the per-thread private copy created by the
  array-section reduction (so `OMP_NUM_THREADS=T` multiplies gradient memory by roughly
  `T`), and the dense `B` block of `U*I/P`, which dominates all of them on every instance
  in the repo.
- **Settle the determinism/scheduling tradeoff** in `mpi/src/updateLR.cpp`:
  `schedule(guided)` plus an array-section reduction makes floating-point summation order
  nondeterministic run-to-run whenever `OMP_NUM_THREADS > 1`, while the CTest suite
  asserts exact output equality on the hybrid path.
- **Housekeeping**: drop the committed `mpi/.DS_Store`, and resolve `stash@{0}` (a
  superseded OpenMP refactor already landed in better form).
- **Out of scope**: any change to the factorisation algorithm; re-running the original
  cluster benchmarks; adding GoogleTest to this machine.

## Capabilities

### New Capabilities

The project has no specs yet (`openspec list --specs` returns none), so both capabilities
below are introduced here. They are named for durable system behaviour rather than for
this change's work.

- `matrix-factorization`: the solver's observable contract, shared by all three
  implementations — instance parsing and validation, the recommendation output format,
  reproducing the course-provided `.out` files, and invariance of that output to process
  count, grid shape, and thread count. Owns the question "is a run correct?"
- `benchmark-reporting`: how performance evidence is produced and what it must assert —
  which configurations are measured, that every measured run is checked for correctness
  rather than assumed, which timing phases are recorded, and the requirement that
  published data, plots, and report figures derive from the code as shipped.

### Modified Capabilities

None. There are no existing specs under `openspec/specs/` to modify.

## Impact

- **Repository state**: the local clone was 14 commits behind `origin/master` because SSH
  authentication has been broken since the clone, so no fetch had ever succeeded and the
  remote-tracking refs were frozen at clone time. The clone is now synced to `7c2fe14`
  and `origin` uses HTTPS (where `gh` is authenticated); the stale refs were what made
  the June merges invisible.
- **Code**: `compare/scripts/benchmark.py` (correctness checking, recorded
  configuration), `mpi/src/updateLR.cpp` (scheduling), and comments at the four invariant
  sites.
- **CI**: `.github/workflows/{tests,benchmark}.yml` are new to `master`. They are also the
  only place the `serial`/`omp` GoogleTest suites can run, since GoogleTest is absent
  locally and those CMake `tests/` targets cannot configure without it.
- **Data and documents**: `compare/data/*.csv`, `compare/plots/*.png`, `mpi/README.md`,
  and the `serial`/`omp`/`mpi` reports (`.tex` and built `.pdf`).
- **Risk if deferred**: no code is at risk — it is all on `master`. What degrades is
  trust in the numbers: every further change makes the published evidence describe the
  shipped code less accurately, with no automation to notice.
