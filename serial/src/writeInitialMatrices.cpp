//
// Created by Pedro Rio on 14/03/2020.
//

#include "writeInitialMatrices.h"

void writeInitialMatrices(std::string &matrixFileName, std::vector<std::vector<double>> &A,
                          std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R) {

    std::ofstream matFile(matrixFileName);
    std::vector<std::vector<double>> B(L.size(), std::vector<double>(R[0].size()));

    matFile << "Initial matrix A" << std::endl;
    std::cout << "Initial matrix A" << std::endl;
    for (auto & i : A) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Initial matrix L" << std::endl;
    std::cout << "Initial matrix L" << std::endl;
    for (auto & i : L) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Initial matrix R" << std::endl;
    std::cout << "Initial matrix R" << std::endl;
    for (auto & i : R) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    // delete this
    matFile << "Initial matrix B" << std::endl;
    std::cout << "Initial matrix B" << std::endl;

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