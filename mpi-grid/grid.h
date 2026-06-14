#ifndef MPIGRID_GRID_H
#define MPIGRID_GRID_H

#include <mpi.h>

// Contiguous block partition of n items across p ranks (Quinn's BLOCK_LOW / SIZE,
// plus the inverse BLOCK_OWNER). Shared as inline helpers so every translation
// unit can use them without a separate definition.
inline int blockLow(int id, int p, int n)    { return (int) ((long) id * n / p); }
inline int blockSize(int id, int p, int n)   { return blockLow(id + 1, p, n) - blockLow(id, p, n); }
inline int blockOwner(int idx, int p, int n) { return (int) (((long) p * (idx + 1) - 1) / n); }

// A 2D Cartesian process mesh: Pr rows (over users) x Pc columns (over items),
// plus the row and column sub-communicators used for the gradient reductions.
// (reorder is disabled at creation, so a rank's cart id equals its world rank.)
struct Grid {
    int world, nprocs;          // world rank, total ranks
    int Pr, Pc;                 // grid dimensions
    int myRow, myCol;           // this rank's grid coordinates
    MPI_Comm cart, rowComm, colComm;
};

Grid makeGrid();                // build the mesh and the row/column sub-communicators
void freeGrid(Grid &g);

#endif  // MPIGRID_GRID_H
