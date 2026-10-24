#ifndef MPIGRID_FILTERFINALMATRIX_H
#define MPIGRID_FILTERFINALMATRIX_H

#include "cell.h"
#include "config.h"
#include "grid.h"

// Build B = L*R on this cell, mask the known non-zeros, take the per-user argmax
// (combined across the row with MPI_MAXLOC, smallest item index on ties to match
// serial), gather the recommendations to the root in user order, and print one item
// index per user.
void filterFinalMatrix(const Grid &g, const Config &cfg, const Cell &cell,
                       const double *L, const double *R);

#endif  // MPIGRID_FILTERFINALMATRIX_H
