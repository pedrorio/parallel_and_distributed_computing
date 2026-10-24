#ifndef MPIGRID_UPDATELR_H
#define MPIGRID_UPDATELR_H

#include "cell.h"
#include "config.h"
#include "grid.h"

// One gradient-descent iteration on the 2D grid, run sequentially within each rank.
// The rank's partial gradient is accumulated into dL/dR, summed across the ROW
// communicator (dL) and the COLUMN communicator (dR), then applied to L/R. dL/dR are
// caller-allocated scratch (sizes uLocal*features and features*iLocal).
//
// mpi-hybrid/ has the same signature and semantics but expresses this body as orphaned
// OpenMP worksharing, so it must be called from inside a parallel region there. Here
// there is no such requirement: call it directly.
void updateLR(const Grid &g, const Config &cfg, const Cell &cell,
              double *L, double *R, double *dL, double *dR);

#endif  // MPIGRID_UPDATELR_H
