#include "updateLR.h"

#include <mpi.h>

#include <cstddef>

// One gradient step for this rank's cell. Parallelism here is across MPI processes
// only: each rank walks its own non-zeros sequentially, then the partial gradients are
// summed across the grid. The hybrid variant (mpi-hybrid/) splits this same body across
// OpenMP threads instead; keeping the two structurally alike is deliberate, so a change
// made to one is easy to spot as missing from the other.
void updateLR(const Grid &g, const Config &cfg, const Cell &cell,
              double *L, double *R, double *dL, double *dR) {
    int F = cfg.features, uLocal = cell.uLocal, iLocal = cell.iLocal;
    double alpha = cfg.convergence;
    size_t lN = (size_t) uLocal * F, rN = (size_t) F * iLocal;

    // reset the gradient accumulators
    for (size_t x = 0; x < lN; x++) dL[x] = 0.0;
    for (size_t x = 0; x < rN; x++) dR[x] = 0.0;

    // local partial gradient over this rank's non-zeros
    //
    // The guard is redundant in this variant: with no reduction clause, a loop over an
    // empty block simply runs zero iterations. It is kept because mpi-hybrid/ needs it --
    // there lN or rN of zero would form a zero-length array section in a reduction
    // clause, which is invalid OpenMP and faults under libgomp -- and because the two
    // trees are easier to compare when they have the same shape. An empty block cannot
    // own a non-zero, so cell.nnz is 0 whenever either length is 0.
    if (lN > 0 && rN > 0) {
        for (int m = 0; m < cell.nnz; m++) {
            int lu = cell.localUser[m], li = cell.localItem[m];
            double pred = 0.0;
            for (int k = 0; k < F; k++)
                pred += L[(size_t) lu * F + k] * R[(size_t) k * iLocal + li];
            double delta = cell.value[m] - pred;
            for (int k = 0; k < F; k++) {
                dL[(size_t) lu * F + k]      += alpha * (2 * delta * R[(size_t) k * iLocal + li]);
                dR[(size_t) k * iLocal + li] += alpha * (2 * delta * L[(size_t) lu * F + k]);
            }
        }
    }

    // cross-rank reduction: the whole local gradient is complete at this point.
    //
    // INVARIANT: dL goes over the ROW communicator and dR over the COLUMN one, and they
    // are not interchangeable. A user's gradient needs contributions from every item, and
    // the ranks holding this rank's user-rows for other item-blocks are the ones in its
    // grid row; an item's gradient needs every user, which is its grid column. Swapping
    // the two communicators still runs and still converges -- to the wrong answer.
    MPI_Allreduce(MPI_IN_PLACE, dL, uLocal * F, MPI_DOUBLE, MPI_SUM, g.rowComm);
    MPI_Allreduce(MPI_IN_PLACE, dR, F * iLocal, MPI_DOUBLE, MPI_SUM, g.colComm);

    for (size_t x = 0; x < lN; x++) L[x] += dL[x];
    for (size_t x = 0; x < rN; x++) R[x] += dR[x];
}
