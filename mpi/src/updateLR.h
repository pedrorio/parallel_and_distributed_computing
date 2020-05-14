#ifndef SERIAL_UPDATELR_H
#define SERIAL_UPDATELR_H

#include <vector>

void updateLR(std::vector<std::vector<double>> &A,
              std::vector<int> &nonZeroUserIndexes,
              std::vector<int> &nonZeroItemIndexes,
              std::vector<double> &nonZeroElements,
              std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
              std::vector<std::vector<double>> &StoreL, std::vector<std::vector<double>> &StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient);

#endif //SERIAL_UPDATELR_H
