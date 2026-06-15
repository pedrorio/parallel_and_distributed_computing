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
// Hybrid: each rank fans its local work across OpenMP threads. The parallel region is
// hoisted around the whole iteration loop (forked once), updateLR uses orphaned
// worksharing inside it, and MPI stays on the main thread (MPI_THREAD_FUNNELED). The
// default is one thread per rank (pure MPI); opt into the hybrid by exporting
// OMP_NUM_THREADS (one rank per socket, threads within). See README.md for tuning.
//
// The pipeline is split across modules: grid (mesh + sub-communicators),
// readInput (parse + broadcast), distribute (route non-zeros to cells), initialLR
// (deterministic slice init), updateLR (one gradient step), filterFinalMatrix
// (B = L*R, argmax, gather, print).
#include <mpi.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "src/cell.h"
#include "src/config.h"
#include "src/distribute.h"
#include "src/filterFinalMatrix.h"
#include "src/grid.h"
#include "src/initialLR.h"
#include "src/readInput.h"
#include "src/updateLR.h"

int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    Grid grid = makeGrid();

    if (provided < MPI_THREAD_FUNNELED && grid.world == 0)
        std::cerr << "matFact-mpi: warning: MPI lacks THREAD_FUNNELED; threads disabled is safest"
                  << std::endl;

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

    // Default to one thread per rank (pure MPI, the best config on a single node and
    // never oversubscribing); opt into the hybrid by exporting OMP_NUM_THREADS. Tune
    // ranks x threads x placement deliberately in the job script -- see README.md.
#ifdef _OPENMP
    if (std::getenv("OMP_NUM_THREADS") == NULL)
        omp_set_num_threads(1);
    int nthreads = omp_get_max_threads();
#else
    int nthreads = 1;
#endif
    if (std::getenv("MATFACT_VERBOSE") != NULL && grid.world == 0)
        std::cerr << "matFact-mpi: grid " << grid.Pr << "x" << grid.Pc
                  << ", " << nthreads << " thread(s)/rank" << std::endl;

    // fork the threads once; updateLR's orphaned worksharing binds to this region
    #pragma omp parallel default(none) shared(grid, cfg, cell, L, R, dL, dR)
    {
        for (int iter = 0; iter < cfg.iterations; iter++)
            updateLR(grid, cfg, cell, L, R, dL, dR);
    }

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
