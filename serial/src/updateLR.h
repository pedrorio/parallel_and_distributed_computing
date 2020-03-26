//
// Created by Pedro Rio on 14/03/2020.
//

#ifndef SERIAL_UPDATELR_H
#define SERIAL_UPDATELR_H

#include <vector>

void updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient);

#endif //SERIAL_UPDATELR_H
