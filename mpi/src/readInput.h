#ifndef SERIAL_READINPUT_H
#define SERIAL_READINPUT_H

#include "mpi.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements, int &processId, int&numberOfProcesses);

#endif //SERIAL_READINPUT_H
