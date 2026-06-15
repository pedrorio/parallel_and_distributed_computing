#ifndef MPIGRID_DISTRIBUTE_H
#define MPIGRID_DISTRIBUTE_H

#include <vector>

#include "cell.h"
#include "config.h"
#include "grid.h"

// Route each global non-zero to its grid cell (userBlock, itemBlock) with a
// bucketed MPI_Scatterv, returning this rank's Cell (block bounds + local
// non-zeros). gUser/gItem/gVal are significant only on the root.
Cell distributeNonZeros(const Grid &g, const Config &cfg,
                        const std::vector<int> &gUser, const std::vector<int> &gItem,
                        const std::vector<double> &gVal);
void freeCell(Cell &c);

#endif  // MPIGRID_DISTRIBUTE_H
