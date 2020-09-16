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

    int numberOfLines;
    std::string line;

    std::ifstream fileLines(inputFileName);
    for (numberOfLines = 0; std::getline(fileLines, line); numberOfLines++) {
        switch (numberOfLines) {
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

                nonZeroElements = new double[numberOfNonZeroElements];
                nonZeroUserIndexes = new int[numberOfNonZeroElements];
                nonZeroItemIndexes = new int[numberOfNonZeroElements];
                A = new double[numberOfUsers * numberOfItems];
                for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
                    A[i] = 0;
                }

                break;
            }
            default: {
                std::istringstream iss(line);
                std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                 std::istream_iterator<std::string>());

                int userIndex = std::stoi(results[0]);
                int itemIndex = std::stoi(results[1]);
                double element = std::stod(results[2]);

                A[userIndex * numberOfItems + itemIndex] = element;

                nonZeroUserIndexes[numberOfLines - 4] = userIndex;
                nonZeroItemIndexes[numberOfLines - 4] = itemIndex;
                nonZeroElements[numberOfLines - 4] = element;
            }
        }
    };
    fileLines.close();
}