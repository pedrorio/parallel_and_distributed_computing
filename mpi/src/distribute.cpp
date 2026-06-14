#include "distribute.h"

#define ROOT 0

Cell distributeNonZeros(const Grid &g, const Config &cfg,
                        const std::vector<int> &gUser, const std::vector<int> &gItem,
                        const std::vector<double> &gVal) {
    Cell c;
    c.userStart = blockLow(g.myRow, g.Pr, cfg.users);
    c.uLocal    = blockSize(g.myRow, g.Pr, cfg.users);
    c.itemStart = blockLow(g.myCol, g.Pc, cfg.items);
    c.iLocal    = blockSize(g.myCol, g.Pc, cfg.items);

    int P = g.nprocs, nnz = cfg.nonZeros;
    int *sendCount = NULL, *displs = NULL, *sU = NULL, *sI = NULL;
    double *sV = NULL;
    if (g.world == ROOT) {
        sendCount = new int[P]();
        for (int m = 0; m < nnz; m++)
            sendCount[blockOwner(gUser[m], g.Pr, cfg.users) * g.Pc + blockOwner(gItem[m], g.Pc, cfg.items)]++;
        displs = new int[P];
        displs[0] = 0;
        for (int p = 1; p < P; p++) displs[p] = displs[p - 1] + sendCount[p - 1];
        std::vector<int> off(displs, displs + P);
        sU = new int[nnz > 0 ? nnz : 1];
        sI = new int[nnz > 0 ? nnz : 1];
        sV = new double[nnz > 0 ? nnz : 1];
        for (int m = 0; m < nnz; m++) {
            int dest = blockOwner(gUser[m], g.Pr, cfg.users) * g.Pc + blockOwner(gItem[m], g.Pc, cfg.items);
            int pos = off[dest]++;
            sU[pos] = gUser[m]; sI[pos] = gItem[m]; sV[pos] = gVal[m];
        }
    }

    MPI_Scatter(sendCount, 1, MPI_INT, &c.nnz, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    c.localUser = new int[c.nnz > 0 ? c.nnz : 1];
    c.localItem = new int[c.nnz > 0 ? c.nnz : 1];
    c.value     = new double[c.nnz > 0 ? c.nnz : 1];
    MPI_Scatterv(sU, sendCount, displs, MPI_INT, c.localUser, c.nnz, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Scatterv(sI, sendCount, displs, MPI_INT, c.localItem, c.nnz, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Scatterv(sV, sendCount, displs, MPI_DOUBLE, c.value, c.nnz, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    // global -> local indices, relative to this cell's block origin
    for (int m = 0; m < c.nnz; m++) { c.localUser[m] -= c.userStart; c.localItem[m] -= c.itemStart; }

    delete[] sendCount;
    delete[] displs;
    delete[] sU;
    delete[] sI;
    delete[] sV;
    return c;
}

void freeCell(Cell &c) {
    delete[] c.localUser;
    delete[] c.localItem;
    delete[] c.value;
}
