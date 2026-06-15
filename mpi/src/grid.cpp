#include "grid.h"

Grid makeGrid() {
    Grid g;
    MPI_Comm_rank(MPI_COMM_WORLD, &g.world);
    MPI_Comm_size(MPI_COMM_WORLD, &g.nprocs);

    int dims[2] = {0, 0};
    MPI_Dims_create(g.nprocs, 2, dims);   // dims[0] = Pr over users, dims[1] = Pc over items
    g.Pr = dims[0];
    g.Pc = dims[1];

    int periods[2] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &g.cart);   // reorder = 0

    int coords[2];
    MPI_Cart_coords(g.cart, g.world, 2, coords);
    g.myRow = coords[0];
    g.myCol = coords[1];

    int keep[2];
    keep[0] = 0; keep[1] = 1; MPI_Cart_sub(g.cart, keep, &g.rowComm);   // same row, varying column
    keep[0] = 1; keep[1] = 0; MPI_Cart_sub(g.cart, keep, &g.colComm);   // same column, varying row
    return g;
}

void freeGrid(Grid &g) {
    MPI_Comm_free(&g.rowComm);
    MPI_Comm_free(&g.colComm);
    MPI_Comm_free(&g.cart);
}
