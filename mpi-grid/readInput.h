#ifndef MPIGRID_READINPUT_H
#define MPIGRID_READINPUT_H

#include <string>
#include <vector>

#include "config.h"
#include "grid.h"

// Root parses and validates the instance file (all ranks MPI_Abort together on a
// bad file) and broadcasts the Config. On the root, gUser/gItem/gVal receive the
// global non-zeros; on the other ranks they stay empty.
void readInput(const std::string &inputFile, const Grid &g, Config &cfg,
               std::vector<int> &gUser, std::vector<int> &gItem, std::vector<double> &gVal);

#endif  // MPIGRID_READINPUT_H
