#include "updateLR.h"

#include <mpi.h>

#include <cstddef>

// Orphaned worksharing: every directive below binds to the parallel region opened in
// matFact.cpp. All threads run this body each iteration; the omp-for constructs split
// the per-rank work, and the implicit barriers keep the threads in lockstep.
void updateLR(const Grid &g, const Config &cfg, const Cell &cell,
              double *L, double *R, double *dL, double *dR) {
    int F = cfg.features, uLocal = cell.uLocal, iLocal = cell.iLocal;
    double alpha = cfg.convergence;
    size_t lN = (size_t) uLocal * F, rN = (size_t) F * iLocal;

    // reset the gradient accumulators
    #pragma omp for schedule(static) nowait
    for (size_t x = 0; x < lN; x++) dL[x] = 0.0;
    #pragma omp for schedule(static)
    for (size_t x = 0; x < rN; x++) dR[x] = 0.0;

    // local partial gradient: each thread builds a private dL/dR, summed at the barrier
    //
    // The guard is required, not an optimisation. When this rank's user-row or
    // item-column block is empty, lN or rN is zero, and a zero-length array section in
    // a reduction clause is not valid OpenMP: libgomp sizes the per-thread private copy
    // from that length and faults at zero (a stack-address SIGSEGV, seen at every
    // process count where a grid row has more ranks than the instance has users), while
    // libomp happens to tolerate it. Skipping costs nothing -- an empty block cannot own
    // a non-zero, so cell.nnz is 0 whenever either length is 0. The condition depends
    // only on this rank's block, so every thread in the team takes the same branch and
    // all of them still encounter the remaining worksharing regions in the same order.
    if (lN > 0 && rN > 0) {
        #pragma omp for schedule(guided) reduction(+ : dL[:lN], dR[:rN])
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

    // cross-rank reduction: only the main thread touches MPI (MPI_THREAD_FUNNELED).
    // The reduction-for above ends with an implicit barrier, so dL/dR are complete here.
    #pragma omp master
    {
        MPI_Allreduce(MPI_IN_PLACE, dL, uLocal * F, MPI_DOUBLE, MPI_SUM, g.rowComm);
        MPI_Allreduce(MPI_IN_PLACE, dR, F * iLocal, MPI_DOUBLE, MPI_SUM, g.colComm);
    }
    #pragma omp barrier   // hold every thread until the reduced gradient is ready

    #pragma omp for schedule(static) nowait
    for (size_t x = 0; x < lN; x++) L[x] += dL[x];
    #pragma omp for schedule(static)
    for (size_t x = 0; x < rN; x++) R[x] += dR[x];
}
