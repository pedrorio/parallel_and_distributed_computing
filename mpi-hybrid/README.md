# `mpi-hybrid` — the 2D-grid solver with OpenMP threads inside each rank

The **MPI + OpenMP hybrid** deliverable. It is the pure MPI solver in
[`mpi/`](../mpi/) with a second level of parallelism added: MPI distributes the problem
across processes, and each process fans its own share across OpenMP threads.

Output matches `mpi/` and the serial version byte-for-byte at any process count, grid
shape **or thread count**, and on repeated runs of the same configuration —
`mpi-hybrid/tests/` asserts all four across 37 cases. Read that as what the suite
checks rather than as a proof: the gradient itself is not bitwise reproducible with
threads on (see [below](#reproducibility-stated-precisely)), so the guarantee rests on
integer arg-max indices being insensitive to that drift, which 37 cases exercise but
cannot establish in principle.

## What this adds over `mpi/`

The 2D-grid decomposition, the partitioning, the row/column reductions, the determinism
of the initialisation and the memory story are all **shared with `mpi/`**, and
[its README](../mpi/README.md) is the reference for them. This file covers only what the
hybrid layer adds:

- **One parallel region per run, not per iteration.** The region is hoisted around the
  whole iteration loop in `matFact.cpp`, so threads are forked once; `updateLR` is written
  as *orphaned* worksharing that binds to it. Forking per iteration would dominate the
  work on these instances.
- **Threads are opt-in.** With `OMP_NUM_THREADS` unset the solver sets one thread per
  rank, so a plain run never oversubscribes and matches `mpi/` in behaviour.
- **MPI stays on one thread.** Start-up requests `MPI_THREAD_FUNNELED` and the two
  `Allreduce` calls sit in an `omp master` region followed by an explicit barrier, so
  only the main thread touches MPI while the others wait for the reduced gradient.
- **The empty-block guard is load-bearing here.** When a rank's block is empty, the
  gradient loop's array-section reduction would have zero length, which is invalid OpenMP
  and faults under libgomp. `mpi/` keeps the same guard, but there it is only tidiness.

### Memory: threads are not free

On top of the per-rank footprint described in [`mpi/`](../mpi/README.md#memory-per-rank),
the gradient loop reduces into **per-thread private copies** of `dL` and `dR`. Each
thread therefore adds about another `(U·F + F·I)/√P` to the rank. On a memory-tight node,
fewer threads per rank may be the only workable split.

### Reproducibility, stated precisely

With threads on, `L` and `R` differ in their last bits between runs: OpenMP does not
specify the order in which per-thread reduction privates are merged, and implementations
merge them as threads finish. Measured on this project, a fixed configuration produced
eight distinct `L`/`R` checksums in eight runs — under `schedule(static)` and
`schedule(guided)` alike, so the loop schedule does not control it.

The published output is unaffected: recommendations are integer argmax indices and are
insensitive to drift that small, which is why the byte-for-byte guarantee above holds
anyway. A bitwise-reproducible gradient would need a fixed-order reduction, which this
solver does not do.

## Module layout

The driver sits at the top level and the modules live in `src/`, mirroring `mpi/` file for file so the two trees stay easy to compare:

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
  matFact.cpp src/*.cpp -o matFact-mpi-hybrid
```

## Run

```sh
# one thread per rank (equivalent to mpi/, useful as the hybrid's own baseline)
mpirun -np <P> ./matFact-mpi-hybrid instance.in

# hybrid: opt into threads explicitly, ideally one rank per socket
OMP_NUM_THREADS=<T> mpirun -np <P> ./matFact-mpi-hybrid instance.in
```

`P` is factored into a balanced `Pr × Pc` automatically (`MPI_Dims_create`); it works at
any `P`, including non-square and primes. `MATFACT_VERBOSE=1` prints the chosen grid and
thread count to stderr.

## Threads (`OMP_NUM_THREADS`) — tuning guidance

The default is **one thread per rank (pure MPI)**, and OpenMP is **explicit opt-in**.
This is deliberate, and it matches what we measured:

- **On a single shared-memory node, pure MPI usually wins** — that is what
  [`mpi/`](../mpi/) is for. Shared-memory MPI is nearly
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
   `(U·F + F·I)/√P` to the rank's footprint (see [Memory per rank](../mpi/README.md#memory-per-rank)).
   On a memory-tight node, fewer threads per rank may be the only workable split.
4. **Pick the split by a short scaling sweep** (a handful of submissions varying
   ranks/threads) and take the knee — as a rule of thumb, denser problems and lower rank
   counts tolerate more threads; sparse/wide problems and high rank counts want fewer.
