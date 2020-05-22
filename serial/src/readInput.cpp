#include "readInput.h"

void readInput(std::string &inputFileName, double *&A,
               int *&nonZeroUserIndexes, int *&nonZeroItemIndexes,
               double *&nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements) {

    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;

    std::vector<std::string> fileCopy;

    std::string line;

    std::ifstream countFileLines(inputFileName);
    for (int numberOfLines = 0; std::getline(countFileLines, line); numberOfLines++) {
        fileCopy.push_back(line);
    };
    countFileLines.close();

    for (int k = 0; k < 4; k++) {
        line = fileCopy[k];
        switch (k) {
            case 0: {
                numberOfIterations = std::stoi(line);
                break;
            }
            case 1: {
                convergenceCoefficient = std::stod(line);
                break;
            }
            case 2: {
                numberOfFeatures = std::stoi(line);
                break;
            }
            case 3: {
                std::istringstream iss(line);

                std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                 std::istream_iterator<std::string>());

                numberOfUsers = std::stoi(results[0]);
                numberOfItems = std::stoi(results[1]);
                numberOfNonZeroElements = std::stoi(results[2]);
                break;
            }
        }
    }

    nonZeroElements = new double[numberOfNonZeroElements];
    nonZeroUserIndexes = new int[numberOfNonZeroElements];
    nonZeroItemIndexes = new int[numberOfNonZeroElements];

    for (int i = 0; i < numberOfNonZeroElements; i++) {
        nonZeroElements[i] = 0;
        nonZeroUserIndexes[i] = 0;
        nonZeroItemIndexes[i] = 0;
    }

    A = new double[numberOfUsers * numberOfItems];
    for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
        A[i] = 0;
    }

    for (int m = 0; m < numberOfNonZeroElements; m++) {
        line = fileCopy[m + 4];
        std::istringstream iss(line);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                         std::istream_iterator<std::string>());
        int userIndex = std::stoi(results[0]);
        int itemIndex = std::stoi(results[1]);
        double element = std::stod(results[2]);

        A[userIndex * numberOfItems + itemIndex] = element;

        nonZeroUserIndexes[m] = userIndex;
        nonZeroItemIndexes[m] = itemIndex;
        nonZeroElements[m] = element;
    }
}