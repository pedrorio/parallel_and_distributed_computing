# Design

## Context

See `proposal.md` for why the root README changes. Three constraints shape the page.

The truth-up design keeps `- [x] MPI` on `README.md`. An older commit flipped that line to unchecked, and landing it would claim the MPI deliverable is unfinished.

`benchmark-reporting` allows a quantitative claim only when `compare/data` supports it. `readInput`, `initialLR`, and `finalFiltering` are written as 0 because those phases are not measured. The end-to-end time is stored in `loop` and copied into `total`. `resultsAreCorrect` counts mismatched recommendations, so 0 means no mismatches.

`solver-variants` keeps `mpi/` and `mpi-hybrid/` as separate trees.

## Goals / Non-Goals

**Goals:**

- One page a hiring manager can read without opening a report.
- A caption whose digits all occur in `compare/data`.

**Non-Goals:**

- Regenerating timings or plots.
- Merging `mpi/` and `mpi-hybrid/`, or adding READMEs for serial and OpenMP.
- Editing `.claude/` or removing OpenSpec files.

## Decisions

### 1. Embed `compare/plots/speedup.line.png`

The line chart shows whether each series keeps improving as cores increase. `speedup.bar.png` encodes the same ratios as bars.

The x axis is cores used. For the hybrid series, `graphs.R` multiplies `numberOfThreads` by `hybridThreadsPerRank` from `compare/data/benchmark-config.json`. The caption states that rule and the recorded value 2, which is how a hybrid point gets its place on the axis.

### 2. Quote recorded times, and let the plot carry the ratio

The caption cites `total` values as written in the CSVs. The ratio is not a column in `compare/data`, and the plot already draws serial time divided by run time.

Name the machine, the repetition count, and the reduction from `benchmark-config.json`. Cite the config's `commit` field so the figure has a provenance label.

### 3. Leave the unmeasured phase columns out of the caption

The caption calls the cited seconds end-to-end wall-clock time. It does not quote `readInput`, `initialLR`, or `finalFiltering`.

If the caption mentions `resultsAreCorrect`, it says that 0 is a mismatch count.

### 4. Extend the checklist by one checked line

Keep `- [x] MPI`, `- [x] Serial`, and `- [x] OpenMP` exactly. Add `- [x] MPI hybrid`, because that variant has its own tree, tests, data file, and report.

Leaving the list at three lines would hide a shipped deliverable. Unchecking MPI would repeat the regression the truth-up design exists to prevent.

### 5. Point at write-ups that exist

Link `mpi/README.md`, `mpi-hybrid/README.md`, and the four `report/main.pdf` files. Serial and OpenMP have reports and no README. The page says so, and does not invent those files.

Build commands are the four `-O3` lines in `.github/workflows/benchmark.yml`. Run commands use `instances/inst0.in`, which is small. MPI and hybrid details stay in their own READMEs.

## Risks / Trade-offs

- A caption digit that is not in `compare/data` would present an unmeasured figure. The verification task rejects any number in `README.md` that those files do not contain.
- `inst0` is a smoke input, not one of the three measured instances. The run section names it as the command argument, and the plot section names the measured instances, so the two are not the same run.
- The checklist grows by one line. The MPI box stays checked, which is the invariant the truth-up design protects.

## Migration Plan

1. Replace `README.md` per the decisions above.
2. Check the caption against `compare/data`, and run `openspec validate`.

Rollback is `git revert` of that commit. No data files move.
