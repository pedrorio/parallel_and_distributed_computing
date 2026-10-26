# Tasks

## 1. Root README

- [x] 1.1 Rewrite `README.md` with the problem, one line each for serial, OpenMP, MPI, and MPI hybrid, the embedded plot `compare/plots/speedup.line.png`, build and run commands, and links to `mpi/README.md`, `mpi-hybrid/README.md`, and the four `report/main.pdf` files. Verify the file contains the exact line `- [x] MPI` and does not quote `readInput`, `initialLR`, or `finalFiltering`.
- [x] 1.2 Check every number in the `README.md` prose, outside fenced commands, against `compare/data` and verify each one occurs in those files. Verify the caption states that `resultsAreCorrect` of 0 means no mismatches, and that the cited seconds are end-to-end wall-clock times.

## 2. Change validation

- [x] 2.1 Run `openspec validate rewrite-root-readme --type change` and verify it exits 0.
