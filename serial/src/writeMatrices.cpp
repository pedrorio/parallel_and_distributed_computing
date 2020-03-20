//
// Created by Pedro Rio on 14/03/2020.
//

#include "writeMatrices.h"

void writeMatrices(std::string &matrixFileName,
                          std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                          std::vector<std::vector<double>> &B, int &iterationNumber) {

    std::ofstream matFile(matrixFileName);

    matFile << "Iter=" << iterationNumber << std::endl;
    std::cout << "Iter=" << iterationNumber << std::endl;

    matFile << "Matrix L" << std::endl;
    std::cout << "Matrix L" << std::endl;
    for (auto & i : L) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Matrix R" << std::endl;
    std::cout << "Matrix R" << std::endl;
    for (auto & i : R) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Matrix B" << std::endl;
    std::cout << "Matrix B" << std::endl;
    for (auto & i : B) {
        for (double j : i) {
            matFile << std::fixed << std::setprecision(6) << j << " ";
            std::cout << std::fixed << std::setprecision(6) << j << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile.close();
};