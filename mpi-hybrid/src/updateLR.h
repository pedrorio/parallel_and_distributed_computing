#ifndef MPIGRID_UPDATELR_H
#define MPIGRID_UPDATELR_H

#include "cell.h"
#include "config.h"
#include "grid.h"

// One gradient-descent iteration on the 2D grid, expressed as ORPHANED OpenMP
// worksharing: it MUST be called from inside an enclosing `#pragma omp parallel`
// region (see matFact.cpp), so the per-rank work is shared across threads while the
// region is forked only once for the whole run. Each thread accumulates a private
// partial of dL/dR (array reduction); the reduced per-rank gradient is then summed
// across the ROW communicator (dL) and COLUMN communicator (dR) by the main thread
// only (MPI_THREAD_FUNNELED) and applied to L/R. dL/dR are caller-allocated scratch
// (sizes uLocal*features and features*iLocal). With one thread this is exactly the
// pure-MPI iteration.
void updateLR(const Grid &g, const Config &cfg, const Cell &cell,
              double *L, double *R, double *dL, double *dR);

#endif  // MPIGRID_UPDATELR_H
