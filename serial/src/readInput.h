//
// Created by Pedro Rio on 14/03/2020.
//

#ifndef SERIAL_READINPUT_H
#define SERIAL_READINPUT_H

#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfLatentFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements);

#endif //SERIAL_READINPUT_H
