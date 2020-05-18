#include "readInput.h"

#define ROOT 0

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n) - 1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n) + 1)
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))

void readInput(std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<int> &nonZeroUserIndexes, std::vector<int> &nonZeroItemIndexes,
               std::vector<double> &nonZeroElements,
               int &numberOfIterations, int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers,
               int &numberOfItems, int &numberOfNonZeroElements, int &processId, int &numberOfProcesses) {


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

    MPI_Bcast(&numberOfUsers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfItems, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    std::vector<std::vector<double>> ResizeA(numberOfUsers, std::vector<double>(numberOfItems, 0));
    A = ResizeA;

    std::vector<int> ResizeIndexes(numberOfNonZeroElements);
    nonZeroUserIndexes = ResizeIndexes;
    nonZeroItemIndexes = ResizeIndexes;

    std::vector<double> ResizeElements(numberOfNonZeroElements);
    nonZeroElements = ResizeElements;

    // TODO
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

    // TODO
    MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    // TODO: DIVIDE THIS
    MPI_Scatter(&nonZeroUserIndexes[BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements)],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_INT, &nonZeroUserIndexes[0],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Scatter(&nonZeroItemIndexes[BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements)],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_INT, &nonZeroItemIndexes[0],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Scatter(&nonZeroElements[BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements)],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_INT, &nonZeroElements[0],
                BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements), MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    std::vector<std::vector<double>> StoreA(numberOfUsers, std::vector<double>(numberOfItems, 0));
    for (int l = 0; l < BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements); l++) {
        StoreA[nonZeroUserIndexes[l]][nonZeroItemIndexes[l]] = nonZeroElements[l];
    }

    MPI_Allreduce(&StoreA[0][0], &A[0][0], numberOfUsers * numberOfItems, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    printf("process %d ended readInput\n", processId);

    MPI_Barrier(MPI_COMM_WORLD);

}
