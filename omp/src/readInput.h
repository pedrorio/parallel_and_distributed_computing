#ifndef OMP_READINPUT_H
#define OMP_READINPUT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

void readInput(std::string &inputFileName, double *&A,
               int *&nonZeroUserIndexes, int *&nonZeroItemIndexes,
               double *&nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements);

#endif //OMP_READINPUT_H