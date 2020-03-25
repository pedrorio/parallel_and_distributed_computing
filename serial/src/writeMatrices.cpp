//
// Created by Pedro Rio on 14/03/2020.
//

#include "writeMatrices.h"

void writeMatrices(std::string &matrixFileName,
                   std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                   int &iterationNumber) {

    std::ofstream matFile(matrixFileName);

    std::vector<std::vector<double>> B(L.size(), std::vector<double>(R[0].size()));

    matFile << "Iter=" << iterationNumber << std::endl;
    std::cout << "Iter=" << iterationNumber << std::endl;

    matFile << "Matrix L" << std::endl;
    std::cout << "Matrix L" << std::endl;
    for (auto &i : L) {
        for (double k : i) {
            matFile << std::fixed << std::setprecision(6) << k << " ";
            std::cout << std::fixed << std::setprecision(6) << k << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Matrix R" << std::endl;
    std::cout << "Matrix R" << std::endl;
    for (auto &j : R) {
        for (double k : j) {
            matFile << std::fixed << std::setprecision(6) << k << " ";
            std::cout << std::fixed << std::setprecision(6) << k << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Matrix B" << std::endl;
    std::cout << "Matrix B" << std::endl;

    for (int i = 0; i < L.size(); i++) {
        for (int j = 0; j < R[0].size(); j++) {
            for (int k = 0; k < R.size(); k++) {
                B[i][j] += L[i][k] * R[k][j];
            }
            matFile << std::fixed << std::setprecision(6) << B[i][j] << " ";
            std::cout << std::fixed << std::setprecision(6) << B[i][j] << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile.close();
};