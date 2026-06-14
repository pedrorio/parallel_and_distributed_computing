#ifndef MPI_UPDATELR_H
#define MPI_UPDATELR_H

// One gradient-descent iteration, distributed over MPI processes.
//
// Every process holds full copies of L and R. The non-zero entries of A are
// block-partitioned across processes; each process computes the gradient
// contributions of its own block into the increment buffers dL/dR (read-only on
// the snapshots StoreL/StoreR), the contributions are summed across all
// processes with MPI_Allreduce, and the summed increment is applied to the
// snapshot. The result is mathematically identical to the serial update and
// independent of the number of processes.
//
// StoreL, StoreR, dL and dR are caller-allocated scratch buffers
// (StoreL/dL sized numberOfUsers*numberOfFeatures, StoreR/dR sized
// numberOfFeatures*numberOfItems).
void updateLR(double *A,
              int *nonZeroUserIndexes, int *nonZeroItemIndexes,
              double *L, double *R, double *StoreL, double *StoreR,
              double *dL, double *dR,
              int numberOfUsers, int numberOfItems, int numberOfFeatures,
              int numberOfNonZeroElements, double convergenceCoefficient,
              int processId, int numberOfProcesses);

#endif //MPI_UPDATELR_H
