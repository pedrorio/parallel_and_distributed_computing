//
// Created by Pedro Rio on 14/03/2020.
//

#include "readInput.h"


void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements) {
    std::ifstream inFile(inputFileName);
    if (inFile.is_open()) {
        int lineNumber = 1;
        for (std::string line; std::getline(inFile, line);) {
            std::istringstream iss(line);

            switch (lineNumber) {
                case 1: {
                    numberOfIterations = std::stoi(line);
                    break;
                }
                case 2: {
                    convergenceCoefficient = std::stod(line);
                    break;
                }
                case 3: {
                    numberOfFeatures = std::stoi(line);
                    break;
                }
                case 4: {
                    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>());
                    numberOfUsers = std::stoi(results[0]);
                    numberOfItems = std::stoi(results[1]);
                    numberOfNonZeroElements = std::stoi(results[2]);
                    A.resize(numberOfUsers);
                    for (int i = 0; i < numberOfUsers; i++)
                        A[i].resize(numberOfItems, 0);
                    break;
                }
                default: {
                    for (int i = 0; i < numberOfUsers; i++) {
                        for (int j = 0; j < numberOfItems; j++) {
                            std::cout << A[i][j] << " ";
                        }
                        std::cout << std::endl;
                    }
                    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>());
                    int userIndex = std::stoi(results[0]);
                    int itemIndex = std::stoi(results[1]);
                    double element = std::stod(results[2]);
                    A[userIndex][itemIndex] = element;
                    break;
                }
            }
            lineNumber++;
        }
        inFile.close();

        for (int user = 0; user < numberOfUsers; user++) {
            for (int item = 0; item < numberOfItems; item++) {
                if (A[user][item] != 0) {
                    nonZeroElementIndexes.push_back({user, item});
                }
            }
        }
    }
}