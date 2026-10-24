# `mpi` — distributed matrix factorisation on a 2D process grid

The **pure MPI** deliverable: every bit of parallelism here comes from MPI processes, and
each rank runs its share sequentially. It links no OpenMP runtime and ignores thread-count
settings — pure by construction, not by configuration.

It produces the **same recommendations as the serial version**, byte-for-byte identical
output, at any process count and grid shape, and the same output on repeated runs.
`mpi/tests/` asserts all three across 32 cases.

> The MPI + OpenMP hybrid is a separate deliverable in **[`mpi-hybrid/`](../mpi-hybrid/)**.
> It shares this 2D-grid decomposition and adds shared-memory threading inside each rank;
> everything in this file about the grid applies there too.

## Why a 2D grid

A naive **replicate-all** design would keep full copies of `L` and `R` on every rank and
reduce the whole gradient over `MPI_COMM_WORLD` each iteration — so memory is `O(L+R)` per
rank and the dense `Allreduce` becomes a communication ceiling (it *anti-scales* on wide
instances).

`mpi` instead arranges the ranks as a `Pr × Pc` mesh:

- `L` is split by **user-rows** across the grid rows, `R` by **item-columns** across the
  grid columns.
- A non-zero `(u, i)` is owned by rank `(userBlock(u), itemBlock(i))`, so each rank holds
  exactly the `L`-rows and `R`-cols its non-zeros touch — see
  [Memory per rank](#memory-per-rank) for what that costs.
- Each iteration: local partial gradient, then `dL` is summed across the **row**
  communicator and `dR` down the **column** communicator. These reductions move
  `√P`-smaller slices over `√P`-sized sub-communicators, so **communication also falls
  with `P`** instead of hitting a ceiling.
- `B = L·R`, the argmax, and the gather are all distributed too (no serial-root tail).

Determinism is preserved by a *generate-and-discard* init: every rank walks the full
`srandom(1)` sequence in serial order but keeps only its slice.

### Memory per rank

Everything that scales with the problem falls with `P`, but the factor matrices are not
the term that binds:

| allocation | size per rank |
|---|---|
| `L`, `R` | `(U·F + F·I)/√P` |
| `dL`, `dR` (gradient accumulators) | `(U·F + F·I)/√P` again |
| `B = L·R` (dense, in the final filter) | `U·I/P` |

(The hybrid adds a per-thread copy of the gradient on top of this; see its README.)

`B` dominates on every instance in `instances/`: for `inst200-10000-50-100-300` it is
`2,000k` doubles against `510k` for `L+R`; for `inst1000-1000-100-2-30`, `1,000k` against
`200k`. It is inherited from the serial algorithm, which materialises the same dense
matrix, so it is not a regression — but a memory estimate that leaves it out is off by
roughly a factor of five.

The replicate-all comparison above still holds: `U·I/P` and `(U·F + F·I)/√P` both fall
with `P`, while replicate-all's `O(L+R)` per rank does not fall at all.

## Module layout

The driver sits at the top level and the modules live in `src/`:

| file | role |
|---|---|
| `matFact.cpp` | driver: MPI init, pipeline orchestration, the iteration loop |
| `src/grid.{h,cpp}` | `Grid` struct, block-partition helpers, mesh + row/col sub-communicators |
| `src/config.h`, `src/cell.h` | plain data: problem dimensions; this rank's block + local non-zeros |
| `src/readInput.{h,cpp}` | root parse + validate + broadcast |
| `src/distribute.{h,cpp}` | route non-zeros to grid cells (`Scatterv`) |
| `src/initialLR.{h,cpp}` | deterministic slice init |
| `src/updateLR.{h,cpp}` | one gradient step (local partials + row/col `Allreduce`) |
| `src/filterFinalMatrix.{h,cpp}` | `B = L·R`, mask, per-user argmax, gather, print |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
# or directly:
mpicxx -std=c++11 -O3 matFact.cpp src/*.cpp -o matFact-mpi
```

No OpenMP: the build needs an MPI compiler and nothing else.

## Run

```sh
mpirun -np <P> ./matFact-mpi instance.in
```

`P` is factored into a balanced `Pr × Pc` automatically (`MPI_Dims_create`); it works at
any `P`, including non-square and primes, and at process counts larger than the instance
has users or items. `MATFACT_VERBOSE=1` prints the chosen grid to stderr.

Scale by adding processes. There is no thread knob here — `OMP_NUM_THREADS` has no effect
on this binary, by design. If you want threads within a rank, that is
[`mpi-hybrid/`](../mpi-hybrid/).

## Tests

```sh
cmake -S . -B build && cmake --build build && (cd build && ctest)
```

32 cases, no GoogleTest dependency: every instance reproduced at 1, 2 and 4 processes,
non-square and prime grids, empty blocks where processes outnumber users or items,
output invariance across process counts, and the error paths that must abort every rank
rather than deadlock.
