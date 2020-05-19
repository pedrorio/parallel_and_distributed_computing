#include "readInput.h"

#define ROOT 0

#define BLOCK_LOW(id, p, n) ((id)*(n)/(p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id)+1,p,n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n) + 1)
#define BLOCK_OWNER(index, p, n) (((p)*((index)+1)-1)/(n))

void readInput(std::string &inputFileName, double *&A,
               int *&nonZeroUserIndexes, int *&nonZeroItemIndexes,
               double *&nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements, int &processId, int &numberOfProcesses) {

    delete[] A;
    delete[] nonZeroUserIndexes;
    delete[] nonZeroItemIndexes;
    delete[] nonZeroElements;

    std::vector<std::string> fileCopy;
    std::string line;
    int numberOfLines;

    if (processId == ROOT) {
        std::ifstream countFileLines(inputFileName);
        for (numberOfLines = 0; std::getline(countFileLines, line); numberOfLines++) {
            fileCopy.push_back(line);
        };
        countFileLines.close();
    }

    MPI_Bcast(&numberOfLines, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (processId == ROOT) {
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
    }

    MPI_Bcast(&numberOfIterations, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&convergenceCoefficient, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfFeatures, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfUsers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfItems, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    A = new double[numberOfUsers * numberOfItems];
    for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
        A[i] = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    auto *StoreA = new double[numberOfUsers * numberOfItems];

    for (int i = 0; i < numberOfUsers * numberOfItems; i++) {
        StoreA[i] = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    nonZeroUserIndexes = new int[numberOfNonZeroElements];

    MPI_Barrier(MPI_COMM_WORLD);

    nonZeroItemIndexes = new int[numberOfNonZeroElements];

    MPI_Barrier(MPI_COMM_WORLD);

    nonZeroElements = new double[numberOfNonZeroElements];

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < numberOfNonZeroElements; i++) {
        nonZeroUserIndexes[i] = 0;
        nonZeroItemIndexes[i] = 0;
        nonZeroElements[i] = 0;
    }

    if (processId == ROOT) {
        for (int m = 0; m < numberOfNonZeroElements; m++) {
            line = fileCopy[m + 4];
            std::istringstream iss(line);
            std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                             std::istream_iterator<std::string>());
            int userIndex = std::stoi(results[0]);
            int itemIndex = std::stoi(results[1]);
            double element = std::stod(results[2]);

            nonZeroUserIndexes[m] = userIndex;
            nonZeroItemIndexes[m] = itemIndex;
            nonZeroElements[m] = element;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&nonZeroUserIndexes[0],
              numberOfNonZeroElements, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&nonZeroItemIndexes[0],
              numberOfNonZeroElements, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&nonZeroElements[0],
              numberOfNonZeroElements, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    int startIndex = BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements);
    int endIndex = startIndex + BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements);
    for (int l = startIndex; l < endIndex; l++) {
        StoreA[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] = nonZeroElements[l];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allreduce(&StoreA[0], &A[0], numberOfUsers * numberOfItems, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

//    if (processId == 0) {
//        for (int i = 0; i < numberOfUsers; i++) {
//            for (int j = 0; j < numberOfItems; j++) {
//                std::cout << A[i * numberOfItems + j] << " ";
//                fflush(stdout);
//            }
//            std::cout << std::endl;
//            fflush(stdout);
//        }
//    }

    MPI_Barrier(MPI_COMM_WORLD);
}
