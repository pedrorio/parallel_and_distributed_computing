# `mpi` — distributed matrix factorisation on a 2D process grid

A hybrid MPI + OpenMP solver for the recommender factorisation `A ≈ L·R`. It produces
the **same recommendations as the serial version**, byte-for-byte identical output, at
any process count, grid shape or thread count — and the same output on repeated runs of
the same configuration. `mpi/tests/` asserts all four.

That is a guarantee about the *output*, not the arithmetic. With threads on, `L` and `R`
differ in their last bits between runs, because OpenMP does not specify the order in
which per-thread reduction privates are merged. The recommendations are integer argmax
indices and are insensitive to drift that small, which is why the output guarantee holds
anyway. A bitwise-reproducible gradient would need a fixed-order reduction.

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
| per-thread reduction privates | `T × (dL + dR)` — **each thread costs its own copy** |
| `B = L·R` (dense, in the final filter) | `U·I/P` |

`B` dominates on every instance in `instances/`: for `inst200-10000-50-100-300` it is
`2,000k` doubles against `510k` for `L+R`; for `inst1000-1000-100-2-30`, `1,000k` against
`200k`. It is inherited from the serial algorithm, which materialises the same dense
matrix, so it is not a regression — but a memory estimate that leaves it out is off by
roughly a factor of five.

The replicate-all comparison above still holds: `U·I/P` and `(U·F + F·I)/√P` both fall
with `P`, while replicate-all's `O(L+R)` per rank does not fall at all.

## Module layout

The driver sits at the top level and the modules live in `src/`, mirroring `mpi/`:

| file | role |
|---|---|
| `matFact.cpp` | driver: pipeline orchestration + the hoisted OpenMP region |
| `src/grid.{h,cpp}` | `Grid` struct, block-partition helpers, mesh + row/col sub-communicators |
| `src/config.h`, `src/cell.h` | plain data: problem dimensions; this rank's block + local non-zeros |
| `src/readInput.{h,cpp}` | root parse + validate + broadcast |
| `src/distribute.{h,cpp}` | route non-zeros to grid cells (`Scatterv`) |
| `src/initialLR.{h,cpp}` | deterministic slice init |
| `src/updateLR.{h,cpp}` | one gradient step (orphaned OpenMP worksharing + row/col `Allreduce`) |
| `src/filterFinalMatrix.{h,cpp}` | `B = L·R`, mask, per-user argmax, gather, print |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
# or directly (macOS / Apple clang + libomp):
mpicxx -std=c++11 -O3 -Xclang -fopenmp \
  -I$(brew --prefix libomp)/include -L$(brew --prefix libomp)/lib -lomp \
  matFact.cpp src/*.cpp -o matFact-mpi
```

## Run

```sh
# pure MPI (default: one thread per rank)
mpirun -np <P> ./matFact-mpi instance.in

# hybrid: opt into threads explicitly, ideally one rank per socket
OMP_NUM_THREADS=<T> mpirun -np <P> ./matFact-mpi instance.in
```

`P` is factored into a balanced `Pr × Pc` automatically (`MPI_Dims_create`); it works at
any `P`, including non-square and primes. `MATFACT_VERBOSE=1` prints the chosen grid and
thread count to stderr.

## Threads (`OMP_NUM_THREADS`) — tuning guidance

The default is **one thread per rank (pure MPI)**, and OpenMP is **explicit opt-in**.
This is deliberate, and it matches what we measured:

- **On a single shared-memory node, pure MPI usually wins.** Shared-memory MPI is nearly
  free, while threads add reduction/barrier overhead. More ranks beat more threads — the
  one exception is a wide, communication-bound instance at high rank count, where adding
  ranks would over-communicate; there a few threads/rank help.
- **The hybrid is fundamentally a *cluster* optimisation.** One rank per node/socket ×
  threads-within keeps intra-node work off the network and shrinks the `Pr × Pc` grid (so
  the `Allreduce`s are cheaper). That's where threads earn their keep.

There is **no good runtime formula** for the thread count: the optimum depends on memory
bandwidth, NUMA, the interconnect, and the problem shape — machine-specific quantities a
single run can't know. (An auto-picker based on `T ≈ √(nnz / (uLocal + iLocal))` was
prototyped and measured 20–53 % off the best in half the cases, in non-fixable
directions, so it was dropped.) Instead:

1. **Set `P`, ranks-per-socket, and `OMP_NUM_THREADS` in the job script.** Bind ranks to
   sockets (`srun --cpu-bind`, `--ntasks-per-socket`; `OMP_PROC_BIND=close`,
   `OMP_PLACES=cores`).
2. **Never oversubscribe:** `ranks-per-node × OMP_NUM_THREADS ≤ cores-per-node`.
3. **Count the memory, not just the cores.** Threads are not free: the gradient loop
   reduces into per-thread private copies of `dL` and `dR`, so each thread adds another
   `(U·F + F·I)/√P` to the rank's footprint (see [Memory per rank](#memory-per-rank)).
   On a memory-tight node, fewer threads per rank may be the only workable split.
4. **Pick the split by a short scaling sweep** (a handful of submissions varying
   ranks/threads) and take the knee — as a rule of thumb, denser problems and lower rank
   counts tolerate more threads; sparse/wide problems and high rank counts want fewer.
