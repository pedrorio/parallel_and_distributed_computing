//
// Created by Pedro Rio on 26/03/2020.
//

#ifndef SERIAL_FILTERFINALMATRIX_H
#define SERIAL_FILTERFINALMATRIX_H


#include <vector>

void filterFinalMatrix(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
                       std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures);


#endif //SERIAL_FILTERFINALMATRIX_H
