#include "readInput.h"

#include <cstdlib>
#include <stdexcept>

static void readInputFail(const std::string &message) {
    std::cerr << "readInput: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

void readInput(std::string &inputFileName, double *&A,
               int *&nonZeroUserIndexes, int *&nonZeroItemIndexes,
               double *&nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements) {

    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;

    std::ifstream fileLines(inputFileName);
    if (!fileLines) {
        readInputFail("could not open '" + inputFileName + "'");
    }

    bool headerParsed = false;
    int numberOfLines;
    std::string line;

    try {
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
                    if (results.size() < 3) {
                        readInputFail("dimensions line needs 3 values");
                    }

                    numberOfUsers = std::stoi(results[0]);
                    numberOfItems = std::stoi(results[1]);
                    numberOfNonZeroElements = std::stoi(results[2]);
                    if (numberOfUsers <= 0 || numberOfItems <= 0 || numberOfNonZeroElements < 0) {
                        readInputFail("dimensions must be positive");
                    }

                    nonZeroElements = new double[numberOfNonZeroElements];
                    nonZeroUserIndexes = new int[numberOfNonZeroElements];
                    nonZeroItemIndexes = new int[numberOfNonZeroElements];
                    A = new double[(size_t) numberOfUsers * numberOfItems];
                    for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
                        A[i] = 0;
                    }

                    headerParsed = true;
                    break;
                }
                default: {
                    int index = numberOfLines - 4;
                    if (index >= numberOfNonZeroElements) {
                        readInputFail("more non-zero lines than the declared count");
                    }

                    std::istringstream iss(line);
                    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>());
                    if (results.size() < 3) {
                        readInputFail("non-zero line needs 3 values");
                    }

                    int userIndex = std::stoi(results[0]);
                    int itemIndex = std::stoi(results[1]);
                    double element = std::stod(results[2]);
                    if (userIndex < 0 || userIndex >= numberOfUsers ||
                        itemIndex < 0 || itemIndex >= numberOfItems) {
                        readInputFail("non-zero index out of range");
                    }

                    A[(size_t) userIndex * numberOfItems + itemIndex] = element;

                    nonZeroUserIndexes[index] = userIndex;
                    nonZeroItemIndexes[index] = itemIndex;
                    nonZeroElements[index] = element;
                }
            }
        }
    } catch (const std::exception &e) {
        readInputFail(std::string("malformed instance file (") + e.what() + ")");
    }

    fileLines.close();

    if (!headerParsed) {
        readInputFail("file ended before the 4 header lines");
    }
    if (numberOfLines - 4 != numberOfNonZeroElements) {
        readInputFail("expected " + std::to_string(numberOfNonZeroElements)
                      + " non-zero lines but found " + std::to_string(numberOfLines - 4));
    }
}
