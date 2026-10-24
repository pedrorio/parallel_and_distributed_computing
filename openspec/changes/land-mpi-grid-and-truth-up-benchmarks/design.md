# Design

## Context

See `proposal.md` — Why. Three facts shape the approach:

1. **The work is already on `master`, and the local view of that was wrong.** The 2D-grid
   solver landed in June through merged PRs #2 and #3. The local clone reported otherwise
   because SSH authentication to GitHub has been broken since the clone — the loaded key
   is not registered with the account — so no fetch ever succeeded and
   `git branch -av` kept showing remote-tracking refs frozen at clone time, 14 commits
   behind. The clone is now synced to `7c2fe14` and `origin` points at HTTPS, where `gh`
   is authenticated with `repo` and `workflow` scopes.
2. **One commit remains unlanded, and it is not a fast-forward.** `a1667ff` ("Add GitHub
   Actions for tests and benchmark") sits on a branch whose other nine commits are already
   in `master`, while `master` carries three commits the branch lacks (the two merge
   commits and a README update). Its content is two workflow files plus one README line
   that must not come with them.
3. **The code is in better condition than the evidence for it.** A read of the MPI
   implementation found no correctness defects in the distribution, reduction, or
   tie-breaking logic. Everything this design changes is a claim, a measurement, or a
   column — not an algorithm.

Local toolchain: Open MPI 5.0.9 and `libomp` via Homebrew (Apple Silicon, Apple clang);
**GoogleTest is absent**, so the `serial`/`omp` CMake test targets cannot configure here.
`mpi/tests/` is CTest-only by design and does run locally.

## Goals / Non-Goals

**Goals:**

- Land the remaining commit's content without regressing anything `master` already has
  right.
- Make every published number reproducible from the code as shipped, by one command.
- Record the invariants that make the 2D-grid solver correct, so a later refactor does
  not silently break them.
- Correct the documentation claims that are wrong or incomplete (memory scaling,
  per-phase timing).

**Non-Goals:**

- Re-deriving or re-validating the factorisation mathematics. The audit treats the
  gradient step as given and checks only its distribution.
- Making the hybrid path bitwise reproducible across *different* grid shapes or process
  counts. Only run-to-run reproducibility at a fixed configuration is in scope.
- Introducing GoogleTest locally, or changing the `serial`/`omp` test strategy.
- Rewriting history on `master`, or re-landing anything the June merges already brought in.

## Decisions

### 1. Land `a1667ff` as two files, not as a commit

Take `.github/workflows/tests.yml` and `.github/workflows/benchmark.yml` from `a1667ff`
onto current `master` as a fresh commit, and leave its `README.md` change behind.

That commit also changes `README.md` from `- [x] MPI` to `- [ ] MPI` — it predates the
June merges, when MPI genuinely was unfinished. `master` now has the line correct, so any
mechanism that carries the whole commit reintroduces a regression.

*Alternatives considered:* cherry-pick the commit (brings the README change, which then
has to be reverted in a follow-up — two commits where one will do); merge the branch
(brings nothing else, since nine of its ten commits are already in `master`, and records a
merge commit for a two-file addition); rebase the branch onto `master` (same README
problem, plus rewriting a branch that is already published at an older tip).

**The workflows do not land as written.** `benchmark.yml` carries a weekly
`schedule: cron "0 6 * * 1"` and a `commit` input defaulting to `true`, and its commit
step runs on both — so every Monday a GitHub-hosted runner would re-measure all three
implementations and push regenerated `compare/data/*.csv` and `compare/plots/*.png` to the
branch, with `[skip ci]`. The `schedule:` trigger is removed and the `commit` input
defaults to `false` before the workflows are committed.

Rationale: the data committed that way is measured on a shared, variable runner — a third
machine, neither the course cluster nor this laptop — and arrives with no provenance
label, which decision 7 and the `benchmark-reporting` requirements "Report figures state
their provenance" and "Measurement configuration is recorded with the data" already
prohibit. The workflow's own header concedes the numbers are "indicative only — not a
substitute for a benchmark on dedicated hardware", so committing them as the published
data contradicts the file's own warning. It also inverts the failure this change exists to
correct: instead of code moving while data stands still, data would move weekly while code
stands still, and `[skip ci]` means those are the only commits on `master` that CI never
checks.

`workflow_dispatch` is kept, because reproducing the measurement on demand is useful, and
the existing `upload-artifact` step already publishes the results for inspection without
committing them. Regenerating published data stays a deliberate local act.

### 2. Treat these four properties as load-bearing invariants

Each is currently correct, non-obvious, and would break silently if "cleaned up". They
belong in the `matrix-factorization` spec as requirements, and in code comments where
they are not already:

| Invariant | Why it is fragile |
|---|---|
| `MPI_Cart_create(..., reorder = 0)` | `distribute.cpp` computes a destination as `row * Pc + col` and scatters over `MPI_COMM_WORLD`. That arithmetic is only valid because cart rank equals world rank. Enabling reorder would misroute every non-zero. |
| `dL` reduces over `rowComm`, `dR` over `colComm` | A user's gradient needs all items (same grid row); an item's needs all users (same grid column). Swapping them produces plausible-looking wrong answers, not a crash. |
| `MPI_MAXLOC` carries the **global** item index | Serial keeps the lowest index on a value tie. MAXLOC resolves ties by minimum index, so tie-breaking matches serial only while the reduced index is global, not block-local. |
| `ok` is broadcast before the header | A malformed instance must abort on every rank. Validating only on root and broadcasting the header unconditionally would hang the non-roots in a `Bcast` that never arrives. |

### 3. One correctness oracle, reused — not a second implementation

`benchmark.py` should establish correctness by capturing stdout and comparing against the
instance's `.out` file, recording the **error count** so the `resultsAreCorrect` column
keeps its original meaning (`0` = no mismatches). It currently hardcodes `0` and runs with
`stdout=DEVNULL`.

*Alternative considered:* re-add a `verifyResult` module to the MPI binary and a
`LOG_RESULTS` path, as the pre-June driver had. Rejected — it puts the measurement
apparatus inside the thing being measured, and duplicates the comparison that
`mpi/tests/` already performs correctly.

### 4. Wall-clock only; drop per-phase claims rather than fake them

Keep `benchmark.py`'s single end-to-end measurement and remove per-phase prose from the
reports, instead of restoring four phase columns.

Rationale: in a distributed run a "phase time" is per-rank and must be max-reduced across
ranks to mean anything, and the barrier needed to do that perturbs the very thing being
timed. A single honest end-to-end number is worth more than four ambiguous ones. The
script already documents this choice; the reports have not caught up.

*Trade-off accepted:* the readInput/initialLR/loop/finalFiltering breakdown that the
pre-June CSVs carried is not coming back.

### 5. Keep `schedule(guided)`; the determinism it was going to buy does not exist

The gradient loop keeps `schedule(guided)`. An earlier version of this design changed it
to `schedule(static)` to make the hybrid path reproducible run to run. Measurement refuted
the premise, so the change was reverted.

**What was measured.** A scratch build fingerprinted `L` and `R` after the iteration loop
(per-rank FNV-1a over the raw bit patterns, combined with `MPI_SUM` on `unsigned long
long` so the cross-rank combination could not itself introduce variation), on
`inst1000-1000-100-2-30`, eight runs per configuration:

| configuration | `schedule(static)` | `schedule(guided)` |
|---|---|---|
| np1, T=1 | 1 distinct checksum of 8 | 1 distinct of 8 |
| np1, T=4 | **8 distinct of 8** | 8 distinct of 8 |
| np2, T=4 | **8 distinct of 8** | 8 distinct of 8 |

The loop schedule does not control the nondeterminism. OpenMP does not specify the order
in which per-thread reduction privates are combined into the shared variable, and
implementations merge them as threads finish; that is the whole effect. `static` fixes
which iterations go to which thread and changes nothing about the merge, so it is exactly
as nondeterministic as `guided`. With one thread there is no merge and both are stable.

**Speed is a coin flip**, minimum of three runs per cell:

| instance | np1 T=2 | np1 T=4 | np1 T=8 | np2 T=4 |
|---|---|---|---|---|
| `inst1000-1000-100-2-30` | static +3.3% | static +8.4% | guided +11.6% | guided +12.1% |
| `inst200-10000-50-100-300` | guided +2.9% | static +2.2% | guided +6.6% | guided +5.0% |
| `inst500-500-20-2-100` | static +1.4% | static +2.7% | guided +4.6% | static +0.4% |

`static` wins at low thread counts, `guided` once threads approach the core count, and
most cells are inside the noise of a loaded laptop. Neither dominates.

With the determinism rationale false and the performance evidence neutral, there is no
case for changing working code, so `guided` stays as the original author wrote it.

*What would actually give bitwise reproducibility:* a fixed-order reduction — per-thread
partials written into indexed buffers, then summed in thread order — which is a real
change to the gradient step and is not in this change's scope.

**The exact-output tests are still sound.** They compare printed recommendations, which
are integer argmax indices and insensitive to ~1e-15 of drift in `L` and `R`. The
`matrix-factorization` requirements ask for output invariance and repeat-run output
equality, not a bitwise-identical gradient, and those hold under both schedules.
`mpi.repeatable.np2.t4` is kept: it is the first thing that would notice if output
stability ever broke, and its comment now states what it does and does not assert.

### 6. State the real memory bound

`mpi/README.md`'s `(U*F + F*I)/sqrt(P)` is the smallest of the terms actually allocated:

```
  per rank:
    L, R               (U*F + F*I)/sqrt(P)      <-- the documented figure
    dL, dR             (U*F + F*I)/sqrt(P)      <-- a second copy
    reduction privates  T x (dL + dR)           <-- array-section reduction, per thread
    B (dense)          U*I / P                  <-- dominates on every repo instance

  inst200-10000-50:   L+R = 510k doubles | B = 2,000k   (B ~4x)
  inst1000-1000-100:  L+R = 200k doubles | B = 1,000k   (B ~5x)
```

Both `U*I/P` and `(U*F + F*I)/sqrt(P)` fall with `P`, so the README's *argument* against
replicate-all survives; only the stated figure is wrong. The per-thread multiplier is the
actionable part, because the tuning section recommends threads without mentioning that
each one costs a private copy of the gradient.

### 7. Keep both datasets in the reports, labelled by machine

The reports' tables keep the original course-cluster rows *and* carry the locally
regenerated rows, with every table naming the machine each row was measured on (course
cluster vs this Apple Silicon laptop) in a column or caption.

Rationale: the cluster rows cover instances that are not in `instances/`
(`instML1M`, `instML100k`, `inst50000-5000-100-2-5`, `inst400-50000-30-200-500`,
`inst600-10000-10-40-400`) and carry the large-instance evidence that makes the scaling
argument strongest. Dropping them would shrink what the reports can claim; presenting them
unlabelled alongside laptop numbers would imply the two are comparable.

*Alternatives considered:* re-scope the tables to the seven instances present here (every
figure reproducible by one command, but the large-instance evidence is lost); obtain or
regenerate the five missing instances (the MovieLens-derived ones may not be obtainable,
and large runs on a laptop cost hours).

*Consequence for decision 3 and the data pipeline:* `compare/data/*.csv` remains
laptop-only and fully reproducible. The cluster rows live in the reports alone and are
explicitly marked as not regenerable from this repository, so the "published data derives
from the code as shipped" requirement applies to the laptop rows and the plots, while the
cluster rows carry a provenance label instead.

## Measured Outcomes

Recorded here because these are findings, not decisions, and the report work depends on
them. All figures: this laptop (macOS 15.6, arm64, 10 cores), minimum of three runs,
`compare/data/benchmark-config.json` carries the full configuration.

### The report's grid figures hold up

The regenerated grid times agree with the hardcoded `tab:grid-vs-rep` grid row within
about 10% at every point, with the same shape:

| instance | np1 | np2 | np4 | np8 |
|---|---|---|---|---|
| `inst200-10000` report / measured | 6.42 / 6.53 | 3.73 / 4.00 | 2.22 / 2.04 | 4.25 / 3.52 |
| `inst500-500` report / measured | 8.32 / 7.97 | 4.40 / 4.28 | 2.62 / 2.25 | 2.64 / 2.15 |
| `inst1000-1000` report / measured | 4.16 / 3.84 | 2.32 / 2.20 | 1.26 / 1.23 | 1.42 / 1.15 |

So the measurement corroborates the report rather than contradicting it, and the
pre-committed rule (report yields to data) does not have to be exercised for these rows.

### One narrative claim is too strong

The report says replicate-all "regresses past 4 processes ... while the grid version keeps
scaling." The grid version *also* regresses at 8 processes on the wide instance —
`2.04 s → 3.52 s` measured, and `2.22 → 4.25` in the report's own numbers. It plateaus
rather than regresses on the other two (`2.25 → 2.15`, `1.23 → 1.15`). The honest claim is
that the grid regresses far *less* (3.52 s against replicate-all's 10.64 s), not that it
keeps scaling. Corrected in the report.

### OpenMP at one thread is slower than serial

| instance | serial | omp T=1 | omp T=4 |
|---|---|---|---|
| `inst1000-1000` | 4.39 | 4.77 (0.92x) | 1.69 (2.6x) |
| `inst500-500` | 8.39 | 10.35 (0.81x) | 4.02 (2.09x) |
| `inst200-10000` | 6.60 | 7.22 (0.91x) | 3.30 (2.00x) |

Expected — a one-thread parallel run pays fork/join and reduction overhead for no
parallelism — and the OpenMP report already says a one-thread run "sits near 1.0". At
0.81x on `inst500-500` that wording is optimistic, so the report now gives the measured
range instead. Note this also qualifies commit 2e5550d's message ("now faster than
serial"): true from two threads up, not at one.

### Both parallel versions regress at 8 workers

Every np8/T8 cell is worse than or level with its np4/T4 counterpart. The machine has 10
cores but only 8 performance cores, so an 8-worker run competes with the OS and the
measurement harness for the last of them. Treated as a property of this machine rather
than of the designs, and noted in the report captions.

## Risks / Trade-offs

- **The CI landing silently reintroduces the README regression** → decision 1 takes the
  two workflow files by path rather than the commit, and the landing task verifies that
  `README.md` is untouched before committing.
- **Regenerated numbers contradict the reports' narrative** (grid beating replicate-all,
  OpenMP beating serial) → then the *report* changes, not the data. The measurement is the
  evidence; the prose is the claim. Deciding this in advance is what stops the temptation
  to re-run until the numbers agree.
- **The baseline has never been verified on this machine** → group 1 runs it before any
  code is touched, so a later failure is attributable to this change rather than to
  something that arrived in June.
- **Benchmarking 8-way on a loaded laptop** → `benchmark.py` already takes the minimum of
  `REPS` runs and caps parallelism at `os.cpu_count()`; record the core count alongside
  the data.
- **Argmax tie flipped by floating-point noise** → low probability (integer output, a
  contracting iteration, differences ~1e-15), and decision 5 removes the run-to-run
  component. It remains possible across *different* process counts, which is a stated
  non-goal.
- **CI first runs only after it lands**, and its `serial`/`omp` jobs exercise GoogleTest
  suites that have never run anywhere → treat the first CI run as part of the landing, not
  as a formality, and fix or report what it surfaces.
- **The `.claude/worktrees/` checkout is now redundant** — its only content not on
  `master` is the two workflow files. Once they land it holds nothing unique, but it
  should not be removed before that.

## Migration Plan

1. *(Done)* Switch `origin` to HTTPS and fast-forward local `master` to `7c2fe14`;
   `e78f417` was verified to be an ancestor, so nothing was rewritten.
2. Verify the baseline: build all three implementations, run the MPI CTest suite.
   **Gate:** do not change code on a failure — a failing suite on untouched `master` is a
   finding about `master`, reported before anything is modified.
3. Land the two CI workflow files (decision 1) and confirm the first run.
4. Apply decisions 3, 5 and the documentation corrections (6).
5. Regenerate data and plots at `PARALLELISM="1 2 4 8"`; commit data and plots together so
   they cannot drift apart again.
6. Reconcile the three reports against the regenerated data, with provenance labels
   (decision 7); rebuild the PDFs.
7. Housekeeping: drop `mpi/.DS_Store`; drop `stash@{0}` once confirmed superseded.

**Rollback:** no step risks the solver — it is already on `master` and on the remote.
Steps 3 onward are ordinary commits; `git revert` restores any of them, and
`origin/master` at `7c2fe14` is the reference point.

## Open Questions

- ~~Should the reports' figures be this machine's numbers or the original cluster's?~~
  **Resolved** — both, labelled by machine. See Decisions — 7.
- Whether `schedule(static)` costs measurable throughput on the widest instance
  (`inst200-10000-50-100-300`, the most unevenly distributed). Safely deferred: it changes
  a constant, not the approach, and step 5 produces the data needed to answer it.
