#include <cstring>
#include "readInput.h"

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements, int &processId) {

    std::vector<std::string> fileCopy;

    int i, k, m, numberOfLines;
    std::string line;

    if (processId == 0) {

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

                    std::vector<std::vector<int>> StoreNonZeroElementIndexes(numberOfNonZeroElements,
                                                                             std::vector<int>(2));
                    nonZeroElementIndexes = StoreNonZeroElementIndexes;

                    std::vector<std::vector<double>> StoreA(numberOfUsers, std::vector<double>(numberOfItems, 0));
                    A = StoreA;
                    break;
                }
            }
        }



//    int lineLength = line.length();
//    char lineChars[lineLength + 1];
//    strcpy(lineChars, line.c_str());

//    char sampleChar2D[][];
    std::vector<int> lineLenghts;

        std::transform(fileCopy.begin(), fileCopy.end(),
                       std::back_inserter(lines.begin()),
                       [](std::string &line) {
            return line.data();
        });

//                           std::for_each(fileCopy.begin(),fileCopy.end(), [](const int& n) { std::cout << " " << n; };)

//        MPI_Scatter(&lines.front(), lineLength, MPI_CHAR, &lineChars[0], lineLength, MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Scatter(fileCopy.front().c_str(), lineLength, MPI_CHAR, &lineChars[0], lineLength, MPI_CHAR, 0, MPI_COMM_WORLD);

    }
//    MPI_Gather(
//            void* send_data,
//            int send_count,
//            MPI_Datatype send_datatype,
//            void* recv_data,
//            int recv_count,
//            MPI_Datatype recv_datatype,
//            int root,
//            MPI_Comm communicator)

//    MPI_Allgather
//    void* send_data,
//    int send_count,
//    MPI_Datatype send_datatype,
//    void* recv_data,
//    int recv_count,
//    MPI_Datatype recv_datatype,
//    int root,
//    MPI_Comm communicator)

//    #pragma omp parallel for private(m, line) shared(A, nonZeroElementIndexes, numberOfLines, fileCopy) default(none) schedule(guided)
//        MPI_Scatter(&fileCopy.front(), numberOfLines - 4, MPI_CHAR, &line.front(), numberOfLines - 4, MPI_CHAR, 0, MPI_COMM_WORLD);

//        MPI_Alltoall(&fileCopy.front(), lineLength, MPI_CHAR, &lineChars[0], lineLength, MPI_CHAR, MPI_COMM_WORLD);
//    for (int m = 4; m < numberOfLines; m++) {
//        line = fileCopy[m];
//        std::istringstream iss(line);
//        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
//                                         std::istream_iterator<std::string>());
//        int userIndex = std::stoi(results[0]);
//        int itemIndex = std::stoi(results[1]);
//        double element = std::stod(results[2]);

std::cout << "LINE IS: " << line << " IN PROCESS: " << processId << std::endl;
//    fflush(stdout);
//
//
//
//
//        A[userIndex][itemIndex] = element;
//        nonZeroElementIndexes[m - 4] = {userIndex, itemIndex};
//    }

}