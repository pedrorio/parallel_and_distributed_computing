#ifndef SERIAL_FILTERFINALMATRIX_H
#define SERIAL_FILTERFINALMATRIX_H

#include <vector>
#include <algorithm>

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B,
                       std::vector<int> &nonZeroUserIndexes,
                       std::vector<int> &nonZeroItemIndexes,
                       std::vector<double> &nonZeroElements,
                       std::vector<std::vector<double>> &L,
                       std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       std::vector<int> &BV);


#endif //SERIAL_FILTERFINALMATRIX_H
