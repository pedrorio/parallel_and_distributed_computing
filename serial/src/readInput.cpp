//
// Created by Pedro Rio on 14/03/2020.
//

#include "readInput.h"


void readInput(std::ifstream &inFile, std::vector<std::vector<double>> &A, int &numberOfIterations,
               int &numberOfLatentFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements) {
    if (inFile.is_open()) {
        int lineNumber = 1;
        for (std::string line; std::getline(inFile, line);) {
            std::istringstream iss(line);
//            std::cout << "line " << lineNumber << ": " << line << std::endl;
            switch (lineNumber) {
                case 1: {
                    numberOfIterations = std::stoi(line);
//                    std::cout << "numberOfIterations " << numberOfIterations << std::endl;
                    break;
                }
                case 2: {
                    convergenceCoefficient = std::stod(line);
//                    std::cout << "convergenceCoefficient " << convergenceCoefficient << std::endl;
                    break;
                }
                case 3: {
                    numberOfLatentFeatures = std::stoi(line);
//                    std::cout << "numberOfLatentFeatures " << numberOfLatentFeatures << std::endl;
                    break;
                }
                case 4: {
                    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>());
//                    std::cout << "results[0] " << results[0] << std::endl;
//                    std::cout << "results[1] " << results[1] << std::endl;
//                    std::cout << "results[2] " << results[2] << std::endl;
                    numberOfUsers = std::stoi(results[0]);
                    numberOfItems = std::stoi(results[1]);
                    numberOfNonZeroElements = std::stoi(results[2]);
//                    std::cout << " size " << A.size() << std::endl;
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
    }
}