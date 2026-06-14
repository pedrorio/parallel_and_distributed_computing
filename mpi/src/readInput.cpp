#include "readInput.h"

#include <stdexcept>

#define ROOT 0

void readInput(std::string &inputFileName, double *&A,
               int *&nonZeroUserIndexes, int *&nonZeroItemIndexes,
               double *&nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements, int &processId, int &numberOfProcesses) {

    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;

    // Root reads and fully parses the file into temporaries, recording whether
    // the instance is well-formed. Non-root ranks never touch the filesystem.
    int ok = 1;
    std::vector<int> rootUser, rootItem;
    std::vector<double> rootElem;

    if (processId == ROOT) {
        std::vector<std::string> fileCopy;
        std::ifstream fileLines(inputFileName);
        if (!fileLines) {
            ok = 0;
        } else {
            for (std::string line; std::getline(fileLines, line);) {
                fileCopy.push_back(line);
            }
            fileLines.close();
        }

        try {
            if (!ok || fileCopy.size() < 4) {
                throw std::runtime_error("missing header");
            }
            numberOfIterations = std::stoi(fileCopy[0]);
            convergenceCoefficient = std::stod(fileCopy[1]);
            numberOfFeatures = std::stoi(fileCopy[2]);

            std::istringstream iss(fileCopy[3]);
            std::vector<std::string> dims(std::istream_iterator<std::string>{iss},
                                          std::istream_iterator<std::string>());
            if (dims.size() < 3) {
                throw std::runtime_error("bad dimensions line");
            }
            numberOfUsers = std::stoi(dims[0]);
            numberOfItems = std::stoi(dims[1]);
            numberOfNonZeroElements = std::stoi(dims[2]);

            if (numberOfNonZeroElements < 0 ||
                (int) fileCopy.size() < 4 + numberOfNonZeroElements) {
                throw std::runtime_error("not enough non-zero lines");
            }

            for (int m = 0; m < numberOfNonZeroElements; m++) {
                std::istringstream lss(fileCopy[m + 4]);
                std::vector<std::string> results(std::istream_iterator<std::string>{lss},
                                                 std::istream_iterator<std::string>());
                if (results.size() < 3) {
                    throw std::runtime_error("bad non-zero line");
                }
                rootUser.push_back(std::stoi(results[0]));
                rootItem.push_back(std::stoi(results[1]));
                rootElem.push_back(std::stod(results[2]));
            }
        } catch (const std::exception &) {
            ok = 0;
        }
    }

    // Agree on success BEFORE any rank commits to the data broadcasts below. If
    // the root could not read a valid instance, every rank aborts together
    // instead of leaving the non-root ranks blocked forever in MPI_Bcast.
    MPI_Bcast(&ok, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    if (!ok) {
        if (processId == ROOT) {
            std::cerr << "matFact-mpi: could not read a valid instance from '"
                      << inputFileName << "'" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Bcast(&numberOfIterations, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&convergenceCoefficient, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfFeatures, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfUsers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfItems, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    nonZeroUserIndexes = new int[numberOfNonZeroElements];
    nonZeroItemIndexes = new int[numberOfNonZeroElements];
    nonZeroElements = new double[numberOfNonZeroElements];

    if (processId == ROOT) {
        for (int m = 0; m < numberOfNonZeroElements; m++) {
            nonZeroUserIndexes[m] = rootUser[m];
            nonZeroItemIndexes[m] = rootItem[m];
            nonZeroElements[m] = rootElem[m];
        }
    }

    MPI_Bcast(nonZeroUserIndexes, numberOfNonZeroElements, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(nonZeroItemIndexes, numberOfNonZeroElements, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(nonZeroElements, numberOfNonZeroElements, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    // Every process reconstructs the full sparse matrix A locally. A and all the
    // non-zero arrays are now replicated identically on every rank.
    A = new double[numberOfUsers * numberOfItems];
    for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
        A[i] = 0.0;
    }
    for (int l = 0; l < numberOfNonZeroElements; l++) {
        A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] = nonZeroElements[l];
    }
}
