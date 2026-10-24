# Proposal

## Why

The project's deliverables are organised per technology — `serial/`, `omp/`, `mpi/` — but
`mpi/` ships a single target that is a hybrid MPI + OpenMP solver. It behaves as pure MPI
by default (`OMP_NUM_THREADS` unset gives one thread per rank, which is how every
committed figure was measured), yet it requires OpenMP at build time and presents one
deliverable where the structure implies a pure-MPI one.

Splitting them gives a pure-MPI implementation and a hybrid MPI + OpenMP implementation
that can each be built, tested, measured and written up on its own terms.

This deliberately re-separates what was once separate: at commit `7d82d92` the repository
held `mpi/` (replicate-all, pure MPI) and `mpi-grid/` (the 2D-grid hybrid), and PR #3
(`7bb439b`) consolidated them into one. The duplication that consolidation removed is
being accepted again as the cost of two independent deliverables.

## What Changes

- **`mpi/` becomes a genuinely pure MPI implementation**: no OpenMP directives, no
  `#ifdef _OPENMP` blocks, no `OMP_NUM_THREADS` handling, no `MPI_Init_thread` /
  `MPI_THREAD_FUNNELED`, and no OpenMP in its CMake. `MPI_Init` and plain sequential
  loops inside each rank.
- **New `mpi-hybrid/` carries the hybrid implementation**, essentially today's `mpi/`:
  the hoisted parallel region, orphaned worksharing in `updateLR`, `MPI_THREAD_FUNNELED`
  with MPI confined to the main thread, the empty-block guard, and the thread-tuning
  guidance.
- **Each gets its own `CMakeLists.txt` and CTest suite**, and its own report — though the
  report work itself is sequenced into `land-mpi-grid-and-truth-up-benchmarks` group 8,
  which rewrites all four reports in one pass against this structure rather than editing
  them here and again there. The four pinned-thread
  cases (`mpi.hybrid.*`) and the repeat-run case move to `mpi-hybrid/tests/`; the shared
  correctness, grid-shape, empty-block, cross-process determinism and error cases are
  covered in both suites against the same instances.
- **`instances/` stays shared** — both variants must reproduce the same reference outputs.
- **A parity check keeps the two from silently diverging**: both variants must reproduce
  every instance's `.out`, which is what makes the duplication survivable.
- **Published evidence covers both separately**: `compare/scripts/benchmark.py` and
  `compare/scripts/graphs.R` gain a fourth variant, so pure MPI and hybrid get their own
  rows and `comparison.hybrid.csv`.
- **CI gains the new project** in the `tests.yml` matrix.
- **BREAKING**: the single `matFact-mpi` binary that honours `OMP_NUM_THREADS` is
  replaced by two binaries. Anything setting `OMP_NUM_THREADS` against `mpi/` — a job
  script, a benchmark invocation — must target `mpi-hybrid/` instead, where that variable
  still means something.
- **Out of scope**: changing the 2D-grid algorithm or the gradient step; re-measuring the
  course-cluster instances; sharing sources between the two variants (considered and
  declined, see `design.md`).

## Capabilities

### New Capabilities

- `solver-variants`: which solver implementations the project ships, how each one is
  built and verified independently, and what they must agree on. Owns the question "is
  this variant a complete, self-standing deliverable, and does it still agree with the
  others?"

### Modified Capabilities

None. `openspec/specs/` is still empty: `matrix-factorization` and `benchmark-reporting`
are introduced by the in-flight `land-mpi-grid-and-truth-up-benchmarks` change and have
not been archived yet, so there is no main spec to amend. The requirements here are
written so they do not restate that change's output contract, only the per-variant
obligations that sit on top of it.

## Impact

- **New tree**: `mpi-hybrid/` (driver, `src/`, `tests/`, `report/`).
- **Rewritten**: `mpi/matFact.cpp`, `mpi/src/updateLR.cpp`, `mpi/CMakeLists.txt`,
  `mpi/tests/CMakeLists.txt`, `mpi/README.md`, `mpi/report/main.tex` — each losing its
  hybrid content to the new tree.
- **Tooling**: `compare/scripts/benchmark.py`, `compare/scripts/graphs.R`,
  `.github/workflows/tests.yml`, `.github/workflows/benchmark.yml`.
- **Data**: a new `compare/data/comparison.hybrid.csv`, and the four plots regain a
  series.
- **Sequencing**: this change should land before the report work in
  `land-mpi-grid-and-truth-up-benchmarks` (its group 8), otherwise those reports are
  written once against the single-`mpi/` structure and again after the split.
- **Accepted risk**: roughly 900 lines of solver exist twice. A fix applied to one and
  not the other is the failure mode; the parity requirement and both CI suites are the
  guard.
