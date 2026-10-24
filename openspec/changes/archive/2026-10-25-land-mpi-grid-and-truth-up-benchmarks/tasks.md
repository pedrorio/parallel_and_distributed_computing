# Tasks

## 1. Verify the baseline on synced master

- [x] 1.1 Configure and build the MPI target on current `master` and verify `cmake --build` completes with no errors
- [x] 1.2 Run the MPI CTest suite and verify every test passes, including the non-square-grid, empty-block, and pinned-`OMP_NUM_THREADS` cases
- [x] 1.3 Build the serial and OpenMP binaries directly with `libomp` (the CMake `tests/` targets cannot configure without GoogleTest) and verify each runs an instance from `instances/` and reproduces its `.out` file
- [x] 1.4 Run the same instance at 1, 2, and 4 processes and verify all three outputs are byte-identical to the `.out` file, and that a repeat run at a fixed configuration is byte-identical to itself
- [x] 1.5 Stop and report instead of changing code if any of 1.1-1.4 fails; a failure on untouched `master` is a finding about `master`, not about this change

## 2. Land the CI workflows

- [x] 2.1 Take `.github/workflows/tests.yml` and `.github/workflows/benchmark.yml` from `a1667ff` by path onto `master`, and verify `git status` shows only those two files added and `README.md` unmodified
- [x] 2.2 Verify each workflow's triggers name branches that actually receive pushes, and that both files parse as valid workflow YAML
- [x] 2.3 Remove the weekly `schedule:` trigger from `.github/workflows/benchmark.yml` and default its `commit` input to `false`, and verify neither a `schedule:` trigger nor a default-on auto-commit remains in either workflow
- [x] 2.4 Commit the two workflows on a feature branch off `master` and push the branch, verifying the commit contains only the two workflow files
- [x] 2.5 Open the pull request and verify the first CI run appears and its three matrix jobs' outcome, fixing or reporting what it surfaces (the `serial`/`omp` GoogleTest suites have never run anywhere)

## 3. Fix the empty-block crash found by CI

- [x] 3.1 Add a CTest case for the cheapest empty-block configuration (np8 on `inst0`, a 4x2 grid whose first row owns no users) and verify it fails under libgomp before the fix
- [x] 3.2 Guard the gradient reduction so a rank with an empty block never forms a zero-length array section, and verify np8 and np16 reproduce `inst0.out` under both libgomp and libomp, at one and two threads
- [x] 3.3 Verify the full MPI suite passes under both toolchains and that CI's `mpi` job reports success on the pull request

## 4. Make the benchmark verify correctness

- [x] 4.1 Capture each timed run's stdout in `compare/scripts/benchmark.py` instead of discarding it to `DEVNULL`, and verify a single-instance run still records a timing
- [x] 4.2 Compare captured output against the instance's `.out` file and record the mismatch count in the `resultsAreCorrect` column, replacing the hardcoded `0`, and verify a correct run records `0`
- [x] 4.3 Verify a deliberately wrong result is caught: run the comparison against a mismatched `.out` file and confirm a non-zero count is recorded and surfaced rather than reported as a successful measurement
- [x] 4.4 Record the measuring machine's core count, the worker counts, and the repetition count alongside the data, and verify they appear in the script's output

## 5. Settle hybrid determinism

- [x] 5.1 Measure whether the loop schedule controls run-to-run reproducibility, by fingerprinting `L` and `R` across repeated runs under both `schedule(static)` and `schedule(guided)`, and record the result in `design.md`
- [x] 5.2 Measure the speed difference between the two schedules across thread and process counts on three instances, and record it in `design.md`
- [x] 5.3 Decide the schedule on that evidence and verify the MPI CTest suite passes on the decision (kept `schedule(guided)`: static buys no determinism and the speed difference is neutral)
- [x] 5.4 Add a CTest case that runs an instance twice at a fixed process and thread count and fails on any output difference, and verify it passes
- [x] 5.5 Document in the test and in `design.md` that output stability holds while the gradient itself is not bitwise reproducible, so the assertion is not mistaken for a determinism guarantee

## 6. Correct the documentation claims

- [x] 6.1 Replace the memory bound in `mpi/README.md` with one that accounts for `dL`/`dR`, the per-thread private reduction copies, and the dense `B` block of `U*I/P`, and verify the stated figures match the worked instance numbers in `design.md`
- [x] 6.2 Add to the `mpi/README.md` tuning section that each thread costs a private copy of the gradient, and verify the guidance no longer recommends threads without naming their memory cost
- [x] 6.3 Document the four load-bearing invariants (`reorder = 0` and the rank arithmetic, the `rowComm`/`colComm` split, MAXLOC carrying the global item index, the `ok`-before-header broadcast) as code comments where not already present, and verify each named invariant appears at its site

## 7. Regenerate the performance data

- [x] 7.1 Run `compare/scripts/benchmark.py` at `PARALLELISM="1 2 4 8"` against the current binaries and verify it writes all three `compare/data/comparison.*.csv` files with a recorded mismatch count of `0` on every row
- [x] 7.2 Regenerate the four plots with `compare/scripts/graphs.R` and verify each renders from the regenerated data without error
- [x] 7.3 Commit the regenerated data and plots in a single commit and verify the commit contains both, so they cannot drift apart
- [x] 7.4 Compare the regenerated MPI figures against the grid-vs-replicate table in `mpi/report/main.tex` and record whether the measurement supports the report's narrative, without re-running to chase an agreeable result
- [x] 7.5 Answer the deferred `schedule(static)` question and record the finding in `design.md` (settled in group 5 by a direct static-vs-guided sweep, which is stronger evidence than inferring it from the regenerated figures)

## 8. Reconcile the reports

- [x] 8.1 Give `mpi/report` the pure-MPI write-up and `mpi-hybrid/report` its own, covering what the hybrid layer adds, and verify neither report describes a mechanism its variant lacks
- [x] 8.2 Label every table and plot in all four reports with the machine it was measured on, marking the course-cluster figures as not regenerable from this repository, and verify no table is left without an attribution
- [x] 8.3 Add the locally regenerated rows alongside the cluster rows, and verify each local figure traces to a row in the committed comparison data
- [x] 8.4 Verify no speedup, ratio or conclusion in any report is computed by comparing a cluster figure against a local one
- [x] 8.5 Replace the hardcoded grid-vs-replicate figures in the MPI report with the measured values, keeping the replicate-all row labelled as pre-rewrite and not regenerable, and correct the claim that the grid "keeps scaling" -- it also regresses at 8 processes on the wide instance, just far less
- [x] 8.6 Verify no report presents a per-phase timing breakdown as measured data, since the benchmark records only end-to-end wall clock, and remove any that does
- [x] 8.7 Rebuild all four report PDFs and verify each builds and contains the updated tables

## 9. Repository housekeeping

- [x] 9.1 Remove the committed `mpi/.DS_Store` and verify it is both deleted and ignored
- [x] 9.2 Confirm `stash@{0}` is superseded by comparing it against the landed `omp/src/updateLR.cpp`, then drop it by its captured SHA, and verify `git stash list` no longer carries that entry

## 10. Final integration check

- [x] 10.1 Build all three implementations from current `master` and run every available test suite, verifying the MPI suite passes in full
- [x] 10.2 Verify every published figure is accounted for: each local figure traces to committed data, each cluster figure carries its provenance label
- [x] 10.3 Verify CI passes on the final state, so the automation that prevents the next drift is known to work

## Workflow follow-up

- Archive the change once the landed work has been reviewed.
- Verify the archived result.
