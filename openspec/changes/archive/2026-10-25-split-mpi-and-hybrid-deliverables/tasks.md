# Tasks

## 1. Create the hybrid tree from the current implementation

- [x] 1.1 Copy `mpi/` to `mpi-hybrid/` with no behavioural change, renaming the CMake project and target so the two can coexist, and verify `mpi-hybrid` configures and builds
- [x] 1.2 Run the copied suite and verify all 37 cases pass, establishing that the copy is faithful before anything is stripped
- [x] 1.3 Verify `mpi-hybrid` reproduces a reference output at `OMP_NUM_THREADS=4` and at the default, so both its modes are known good

## 2. Make mpi/ a pure MPI deliverable

- [x] 2.1 Remove the OpenMP directives from `mpi/src/updateLR.cpp`, keeping the empty-block guard with a comment saying what it protects now, and verify the gradient step still reads as a plain sequential loop per rank
- [x] 2.2 Replace `MPI_Init_thread` with `MPI_Init` in `mpi/matFact.cpp` and delete the `#ifdef _OPENMP` blocks, the `omp_set_num_threads` call and the thread-count reporting, and verify no `omp` or `_OPENMP` reference remains in `mpi/`
- [x] 2.3 Drop `find_package(OpenMP REQUIRED)` and the OpenMP link from `mpi/CMakeLists.txt`, and verify the target builds and links with no OpenMP runtime involved
- [x] 2.4 Verify a thread-count environment variable has no effect: run the same instance with it unset and set to 4, and confirm byte-identical output and no thread-related behaviour
- [x] 2.5 Verify `mpi/` still reproduces every instance's reference output at np 1/2/4, including non-square grids and empty blocks

## 3. Split the test suites

- [x] 3.1 Remove the four `mpi.hybrid.*` cases and `mpi.repeatable.np2.t4` from `mpi/tests/CMakeLists.txt`, and verify the remaining 32 cases pass against the pure variant
- [x] 3.2 Verify `mpi-hybrid/tests/` retains the five thread-dependent cases plus the shared ones, and that all 37 pass against the hybrid variant
- [x] 3.3 Verify both suites resolve instances from the shared `instances/` directory, with no per-variant copies
- [x] 3.4 Verify each variant builds and tests with the other directory absent, by configuring and running each suite from a tree with the sibling moved aside

## 4. Extend the measurement pipeline to four variants

- [x] 4.1 Add a hybrid binary and a `comparison.hybrid.csv` output to `compare/scripts/benchmark.py`, measuring the hybrid at a thread count above one, and verify a short run writes all four CSVs with correctness recorded for each
- [x] 4.2 Add `"hybrid"` to the `programs` vector in `compare/scripts/graphs.R` and verify all four plots render with the new series
- [x] 4.3 Add `mpi-hybrid` to the `tests.yml` matrix and build the fourth binary in `benchmark.yml`, and verify both workflows parse and that CI runs the new project
- [x] 4.4 Verify the hybrid's recorded configuration states its thread count, so a hybrid row is never mistaken for a pure-MPI row

## 5. Regenerate the evidence for all four variants

- [x] 5.1 Run the benchmark across all four variants at the established worker counts and verify every row records zero mismatches
- [x] 5.2 Regenerate the plots and commit data and plots in a single commit so they cannot describe different code
- [x] 5.3 Compare the pure-MPI figures against the pre-split MPI figures and record whether removing the threading layer changed measured performance, which it should not have

## 6. Split the documentation

- [x] 6.1 Rewrite `mpi/README.md` for the pure variant: its parallelism model, build and run commands, and what distinguishes it from `mpi-hybrid/`, with the thread-tuning guidance removed
- [x] 6.2 Write `mpi-hybrid/README.md` covering what the hybrid adds — the threading model, `MPI_THREAD_FUNNELED`, thread tuning and the per-thread memory cost — referencing rather than restating the shared 2D-grid decomposition
- [x] 6.3 Verify neither README documents a mechanism its own variant lacks

## 7. Final integration check

- [x] 7.1 Build all four variants from a clean state and run every test suite, verifying each passes
- [x] 7.2 Verify every published figure names the variant that produced it and that no comparison mixes variants measured differently
- [x] 7.3 Verify CI is green on the final structure for all four projects

## Workflow follow-up

- Return to `land-mpi-grid-and-truth-up-benchmarks` group 8 and write the reports against this structure.
- Archive this change once the split has been reviewed.
