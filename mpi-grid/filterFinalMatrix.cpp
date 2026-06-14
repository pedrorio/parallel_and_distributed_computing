#include "filterFinalMatrix.h"

#include <mpi.h>

#include <iostream>

void filterFinalMatrix(const Grid &g, const Config &cfg, const Cell &cell,
                       const double *L, const double *R) {
    int F = cfg.features, U = cfg.users;
    int uLocal = cell.uLocal, iLocal = cell.iLocal;

    // B = L*R on this cell (each user owns its B-row -> thread over users), then mask
    double *B = new double[(size_t) uLocal * iLocal + 1];
    #pragma omp parallel for schedule(static)
    for (int lu = 0; lu < uLocal; lu++) {
        double *Brow = B + (size_t) lu * iLocal;
        for (int li = 0; li < iLocal; li++) Brow[li] = 0.0;
        for (int k = 0; k < F; k++) {
            double lik = L[(size_t) lu * F + k];
            for (int li = 0; li < iLocal; li++)
                Brow[li] += lik * R[(size_t) k * iLocal + li];
        }
    }
    for (int m = 0; m < cell.nnz; m++)
        B[(size_t) cell.localUser[m] * iLocal + cell.localItem[m]] = 0.0;

    // per-user argmax over the local item-block, then combine across the row
    struct DblInt { double v; int idx; };
    DblInt *pair = new DblInt[uLocal > 0 ? uLocal : 1];
    #pragma omp parallel for schedule(static)
    for (int lu = 0; lu < uLocal; lu++) {
        double mx = 0.0;
        int arg = 0;
        for (int li = 0; li < iLocal; li++) {
            double b = B[(size_t) lu * iLocal + li];
            if (b > mx) { mx = b; arg = cell.itemStart + li; }   // strict > => smallest item on ties
        }
        pair[lu].v = mx;
        pair[lu].idx = arg;
    }
    MPI_Allreduce(MPI_IN_PLACE, pair, uLocal, MPI_DOUBLE_INT, MPI_MAXLOC, g.rowComm);

    int *bvLocal = new int[uLocal > 0 ? uLocal : 1];
    for (int lu = 0; lu < uLocal; lu++) bvLocal[lu] = pair[lu].idx;

    // gather block recommendations (column 0 of each grid row) to root, in user order
    MPI_Comm col0;
    MPI_Comm_split(MPI_COMM_WORLD, (g.myCol == 0) ? 0 : MPI_UNDEFINED, g.myRow, &col0);
    if (g.myCol == 0) {
        int c0rank;
        MPI_Comm_rank(col0, &c0rank);
        int *rc = NULL, *rd = NULL, *bv = NULL;
        if (c0rank == 0) {
            rc = new int[g.Pr];
            rd = new int[g.Pr];
            for (int r = 0; r < g.Pr; r++) rc[r] = blockSize(r, g.Pr, U);
            rd[0] = 0;
            for (int r = 1; r < g.Pr; r++) rd[r] = rd[r - 1] + rc[r - 1];
            bv = new int[U];
        }
        MPI_Gatherv(bvLocal, uLocal, MPI_INT, bv, rc, rd, MPI_INT, 0, col0);
        if (c0rank == 0) {
            for (int u = 0; u < U; u++) std::cout << bv[u] << std::endl;
            delete[] rc;
            delete[] rd;
            delete[] bv;
        }
        MPI_Comm_free(&col0);
    }

    delete[] B;
    delete[] pair;
    delete[] bvLocal;
}
