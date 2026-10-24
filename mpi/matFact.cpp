// Distributed matrix factorisation on a 2D process grid (driver).
//
// Ranks form a Pr x Pc Cartesian mesh. L is partitioned by user-rows across the grid
// rows and R by item-columns across the grid columns; a non-zero (u,i) is owned by
// rank (userBlock(u), itemBlock(i)), so each rank holds exactly the L-rows and R-cols
// its non-zeros need. Per iteration dL is summed across the ROW communicator and dR
// across the COLUMN communicator -- small reductions over sqrt(P)-sized
// sub-communicators rather than a full WORLD reduce of the whole gradient -- so memory
// and communication both fall with P. The recommendations match the serial output.
//
// This is the pure MPI deliverable: all parallelism comes from MPI processes, and each
// rank runs its share sequentially. There is no shared-memory threading here, by
// construction rather than by default -- it does not link an OpenMP runtime and ignores
// thread-count settings. The MPI + OpenMP hybrid lives in mpi-hybrid/.
//
// The pipeline is split across modules: grid (mesh + sub-communicators),
// readInput (parse + broadcast), distribute (route non-zeros to cells), initialLR
// (deterministic slice init), updateLR (one gradient step), filterFinalMatrix
// (B = L*R, argmax, gather, print).
#include <mpi.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "src/cell.h"
#include "src/config.h"
#include "src/distribute.h"
#include "src/filterFinalMatrix.h"
#include "src/grid.h"
#include "src/initialLR.h"
#include "src/readInput.h"
#include "src/updateLR.h"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    Grid grid = makeGrid();

    if (argc < 2) {
        if (grid.world == 0) std::cerr << "usage: matFact-mpi <instance.in>" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    Config cfg;
    std::vector<int> gUser, gItem;
    std::vector<double> gVal;
    readInput(argv[1], grid, cfg, gUser, gItem, gVal);

    Cell cell = distributeNonZeros(grid, cfg, gUser, gItem, gVal);

    double *L = new double[(size_t) cell.uLocal * cfg.features + 1];
    double *R = new double[(size_t) cfg.features * cell.iLocal + 1];
    initialLR(cfg, cell, L, R);

    double *dL = new double[(size_t) cell.uLocal * cfg.features + 1];
    double *dR = new double[(size_t) cfg.features * cell.iLocal + 1];

    if (std::getenv("MATFACT_VERBOSE") != NULL && grid.world == 0)
        std::cerr << "matFact-mpi: grid " << grid.Pr << "x" << grid.Pc << std::endl;

    for (int iter = 0; iter < cfg.iterations; iter++)
        updateLR(grid, cfg, cell, L, R, dL, dR);

    filterFinalMatrix(grid, cfg, cell, L, R);

    delete[] L;
    delete[] R;
    delete[] dL;
    delete[] dR;
    freeCell(cell);
    freeGrid(grid);

    MPI_Finalize();
    return 0;
}
