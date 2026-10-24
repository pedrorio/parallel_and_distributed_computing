# Design

## Context

See `proposal.md` — Why. The starting point:

- `mpi/` is one CMake target, `matFact-mpi`, with `find_package(OpenMP REQUIRED)`. The
  driver calls `MPI_Init_thread(..., MPI_THREAD_FUNNELED, ...)`, guards its OpenMP use
  behind `#ifdef _OPENMP`, and calls `omp_set_num_threads(1)` when `OMP_NUM_THREADS` is
  unset. So it already *runs* as pure MPI by default, and every committed MPI figure was
  measured that way.
- `mpi/src/updateLR.cpp` is the only module with OpenMP content: four worksharing loops,
  an array-section reduction, an `omp master` region around the two `MPI_Allreduce`
  calls, and an explicit barrier.
- `mpi/tests/` holds 37 CTest cases. Four pin `OMP_NUM_THREADS` (`mpi.hybrid.*`) and one
  more (`mpi.repeatable.np2.t4`) exists specifically to check output stability with
  threads live. The other 32 are parallelism-agnostic: correctness at np 1/2/4 across
  seven instances, grid shapes, empty blocks, cross-process determinism, error handling.
- `compare/scripts/benchmark.py` and `graphs.R` each enumerate variants in one place
  (`("serial", "omp", "mpi")` and `c("serial", "omp", "mpi")`), and `tests.yml` has a
  `project: [serial, omp, mpi]` matrix.
- The in-flight `land-mpi-grid-and-truth-up-benchmarks` change owns the output contract
  and the evidence rules; this design does not restate them.

## Goals / Non-Goals

**Goals:**

- Two deliverables that can each be built, tested, measured and read on their own.
- A pure-MPI variant that is pure by construction, not by default setting.
- Keep the two from diverging without noticing.

**Non-Goals:**

- Changing the 2D-grid decomposition, the gradient step, or any observable output.
- Sharing sources between the variants (declined — see decision 1).
- Reviving the replicate-all design; the pure-MPI variant is the 2D-grid solver with
  threading removed, not the pre-June implementation.
- Re-measuring the course-cluster instances.

## Decisions

### 1. Two directories with duplicated sources, not two targets over one source tree

`mpi/` and `mpi-hybrid/` each hold a full copy of the solver.

This was chosen by the user over the alternative of one source tree with two CMake
targets (`matFact-mpi` compiled without OpenMP, `matFact-mpi-hybrid` with it), which
would have produced the same two binaries with no duplicated code — the `#ifdef _OPENMP`
guards and the orphaned `omp for` directives compile away cleanly when OpenMP is off.

The trade-off is recorded rather than re-argued: two directories give two deliverables
that are legible and gradeable in isolation, matching how `serial/` and `omp/` are already
organised, at the cost of roughly 900 duplicated lines whose divergence is a live risk.
PR #3 consolidated these same two trees in June to remove exactly that risk, so this
reverses a prior decision deliberately.

*Mitigation, not a cure:* decision 5's parity obligation and two CI suites mean a
behavioural change to one variant alone shows up as a failing test rather than as a
silent difference. Nothing prevents a performance or documentation fix from landing in
one tree only.

### 2. The pure-MPI variant is the current solver with threading removed

`mpi/` keeps the 2D-grid decomposition exactly as it is and deletes its shared-memory
layer: the `omp` directives in `updateLR.cpp`, the `#ifdef _OPENMP` blocks and
`omp_set_num_threads` call in the driver, `MPI_Init_thread` in favour of `MPI_Init`, the
`MATFACT_VERBOSE` thread-count reporting, and `find_package(OpenMP REQUIRED)`.

The empty-block guard in `updateLR.cpp` stays. Without the reduction clause it is no
longer protecting against a zero-length array section, but it still skips work that
cannot exist, and keeping both trees structurally parallel makes divergence easier to
spot.

*Alternative considered:* keep the `#ifdef _OPENMP` guards in `mpi/` and simply not link
OpenMP. Rejected for this variant — the guards are what make it "pure only if you build it
that way", and the requirement is that the pure variant carries no threading at all.

### 3. `mpi-hybrid/` is today's `mpi/`, moved

The hybrid tree starts as a copy of the current implementation with no behavioural
change, so the split cannot be blamed for a regression. Its README keeps the thread-tuning
guidance and the per-thread memory cost, which belong only to the variant that has
threads.

### 4. Tests split by what they exercise, and the shared ones are duplicated

| cases | `mpi/tests/` | `mpi-hybrid/tests/` |
|---|---|---|
| correctness at np 1/2/4, seven instances | yes | yes |
| grid shapes (np3/np8/np9), empty blocks (np8/np16) | yes | yes |
| cross-process determinism (np1 vs np4) | yes | yes |
| error handling (missing arg/file, malformed) | yes | yes |
| `mpi.hybrid.*` pinned-thread cases | no | yes |
| `mpi.repeatable.np2.t4` | no | yes |

The shared cases run in both suites because each variant must be verified on its own
(spec: "Testing one variant alone"), and because a duplicated implementation that is only
tested in one tree is exactly the divergence this design is trying to contain. Both
suites read the same `instances/` files.

### 5. Parity is checked through the shared reference outputs

The two variants are not compared against each other directly; both are compared against
the committed `.out` files. That is sufficient — two implementations that each reproduce
the same reference output agree on what the project publishes — and it avoids a test that
needs both binaries built to run, which would break the "build one variant alone"
requirement.

### 6. The variant list grows in the four places that enumerate it

`benchmark.py` gains a `HYBRID_BIN` and writes `comparison.hybrid.csv`; `graphs.R` adds
`"hybrid"` to its `programs` vector so the plots gain a series; `tests.yml` adds
`mpi-hybrid` to its matrix; `benchmark.yml` builds the fourth binary.

The hybrid is measured at a thread count above one, since measuring it at one thread
would just reproduce the pure-MPI column and tell us nothing new. Pure MPI is measured
across process counts as now.

## Measured Outcomes

### Removing the threading layer did not change `mpi/`'s performance

Comparing the two committed datasets directly suggested it had — up to +53.8% on
`inst500-500-20-2-100` at np8, with deltas ranging to −16.3% elsewhere, inconsistent in
sign. That comparison is not valid: the two datasets were measured in different sessions
while CI watches and builds were running, so it mixes the change with machine load.

A controlled A/B settles it. The pre-split binary was rebuilt from `8db9933` and run
interleaved with the post-split one — same session, alternating, five reps each, minimum
kept — so drift hits both equally:

| instance | np | pre-split | post-split | delta |
|---|---|---|---|---|
| `inst500-500-20-2-100` | 1 | 8.355 | 8.384 | +0.4% |
| `inst500-500-20-2-100` | 4 | 2.719 | 2.443 | −10.1% |
| `inst1000-1000-100-2-30` | 1 | 4.093 | 3.995 | −2.4% |
| `inst1000-1000-100-2-30` | 4 | 1.361 | 1.372 | +0.7% |

Small and mixed in sign: no systematic effect, which is what was expected, since the
pre-split binary already ran one thread per rank by default. The lesson for this project's
evidence rules is narrower and worth keeping: **figures from different measurement
sessions on a working laptop are not comparable at the few-percent level.** Any future
claim of that size needs an interleaved A/B, not two committed datasets subtracted.

## Risks / Trade-offs

- **The two trees diverge** → the central risk, accepted by decision 1. Both CI suites
  run on every push, and a behavioural change to one tree alone fails its own suite.
  Non-behavioural drift (comments, docs, tuning) is not caught by anything.
- **Removing threading from `mpi/` changes its measured performance** → it should not:
  the committed MPI figures were all measured at one thread per rank. Re-measurement after
  the split will confirm or refute that, and the figures are regenerated anyway.
- **The hybrid's empty-block guard is load-bearing; the pure variant's is not** → both
  keep it, so the trees stay structurally comparable, with a comment in each saying what
  it protects there.
- **Report duplication** → the two reports will share their description of the 2D-grid
  decomposition. The hybrid report should cover only what the hybrid adds, and reference
  rather than restate the grid design.
- **Sequencing against the other change** → if its group 8 reports are written first,
  they get rewritten after the split. This change should land first.

## Migration Plan

1. Copy `mpi/` to `mpi-hybrid/` unchanged; confirm it builds and its 37 tests pass before
   touching anything.
2. Strip threading from `mpi/` per decision 2; confirm it builds with no OpenMP and that
   its 32 remaining tests pass.
3. Move the five thread-dependent cases out of `mpi/tests/` and confirm both suites pass.
4. Extend `benchmark.py`, `graphs.R` and both workflows to the fourth variant.
5. Regenerate data and plots covering all four variants; commit data and plots together.
6. Split the documentation and reports: `mpi/` pure-MPI, `mpi-hybrid/` the hybrid layer.
7. Return to `land-mpi-grid-and-truth-up-benchmarks` group 8 and write the reports once,
   against the final structure.

**Rollback:** every step is an ordinary commit on a feature branch; `git revert` restores
the single-`mpi/` structure, and PR #3's consolidated tree remains in history as the
reference for what one-tree looked like.

## Open Questions

- Whether the hybrid report should be a standalone document or an addendum that
  references the pure-MPI report for the decomposition it shares. Safely deferred: it
  changes prose organisation, not the structure, the tests or the data.
