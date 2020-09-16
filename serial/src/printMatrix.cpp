#include "printMatrix.h"

void printMatrix(const std::string name, double *&matrix, int &numberOfRows, int &numberOfColumns) {
    std::cout << name << ": (" << numberOfRows << " x " << numberOfColumns << ")" << std::endl;
    for (int i = 0; i < numberOfRows; i++) {
        for (int j = 0; j < numberOfColumns; j++) {
            std::cout << matrix[i * numberOfColumns + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
};