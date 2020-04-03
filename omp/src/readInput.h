#ifndef SERIAL_READINPUT_H
#define SERIAL_READINPUT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElementss);

#endif //SERIAL_READINPUT_H
