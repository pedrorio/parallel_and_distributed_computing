#include "readInput.h"

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements) {

    std::vector<std::string> fileCopy;

    int i,k, m, numberOfLines;
    std::string line;

    std::ifstream countFileLines(inputFileName);
    for (numberOfLines = 0; std::getline(countFileLines, line); numberOfLines++) {
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

                std::vector<std::vector<int>> StoreNonZeroElementIndexes(numberOfNonZeroElements, std::vector<int>(2));
                nonZeroElementIndexes = StoreNonZeroElementIndexes;

                std::vector<std::vector<double>> StoreA(numberOfUsers, std::vector<double>(numberOfItems, 0));
                A = StoreA;
                break;
            }
        }
    }

//    MPI_Scatter(
//            void* send_data,
//            int send_count,
//            MPI_Datatype send_datatype,
//            void* recv_data,
//            int recv_count,
//            MPI_Datatype recv_datatype,
//            int root,
//            MPI_Comm communicator)

//    MPI_Gather(
//            void* send_data,
//            int send_count,
//            MPI_Datatype send_datatype,
//            void* recv_data,
//            int recv_count,
//            MPI_Datatype recv_datatype,
//            int root,
//            MPI_Comm communicator)

//    #pragma omp parallel for private(m, line) shared(A, nonZeroElementIndexes, numberOfLines, fileCopy) default(none) schedule(guided)
    for (int m = 4; m < numberOfLines; m++) {
        line = fileCopy[m];
        std::istringstream iss(line);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                         std::istream_iterator<std::string>());
        int userIndex = std::stoi(results[0]);
        int itemIndex = std::stoi(results[1]);
        double element = std::stod(results[2]);
        A[userIndex][itemIndex] = element;
        nonZeroElementIndexes[m - 4] = {userIndex, itemIndex};
    }
}