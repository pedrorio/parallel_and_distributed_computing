#include <cstring>
#include "readInput.h"

#define READ_INPUT 123


#define FIRST_ELEMENT(id, p, n) ((id)*(n)/(p))
#define LAST_ELEMENT(id, p, n) (FIRST_ELEMENT((id)+1,p,n)-1)
#define BLOCK_SIZE(id, p, n) (LAST_ELEMENT(id,p,n)-FIRST_ELEMENT(id,p,n)+1)

void readInput(const std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements, int &processId) {

//    MPI_Request requests[4];
//    MPI_Status status[4];

    MPI_Request request;
    MPI_Status status;

    std::vector<std::string> fileCopy;

    int i, k, m, numberOfLines;
    std::string line;

//    MPI_Probe(0, READ_INPUT, MPI_COMM_WORLD, &status[processId]);
//    int lineSize;
//    MPI_Get_count(&status[processId], MPI_CHAR, &lineSize);
//    char lineChars[lineSize];
//    MPI_Irecv(&lineChars, lineSize, MPI_CHAR, 0, READ_INPUT, MPI_COMM_WORLD, &requests[processId]);
//    std::string line = lineChars;

    if (processId != 0) {
        MPI_Probe(0, READ_INPUT, MPI_COMM_WORLD, &status);
        int lineSize;
        MPI_Get_count(&status, MPI_CHAR, &lineSize);
        char lineChars[lineSize];
        MPI_Irecv(&lineChars, lineSize, MPI_CHAR, 0, READ_INPUT, MPI_COMM_WORLD, &request);
        line = std::string(lineChars);

        MPI_Wait(&request, &status);
        //        MPI_Waitall(4, requests, status);
    }

//    if (processId != 0) {
//        MPI_Wait(&requests[processId], &status[processId]);
//    }

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

//        MPI_Isend(fileCopy[4].c_str(), fileCopy[4].size(), MPI_CHAR, 1, READ_INPUT, MPI_COMM_WORLD, &request);

        for (int k = 1; k < 4; k++) {
            MPI_Isend(fileCopy[k + 4].c_str(), fileCopy[k + 4].size(), MPI_CHAR, k, READ_INPUT, MPI_COMM_WORLD,
                      &request);
//            MPI_Isend(&fileCopy[k + 4], fileCopy[k + 4].size(), MPI_CHAR, k, READ_INPUT, MPI_COMM_WORLD, &requests[k]);

        }





//        MPI_Scatter(fileCopy.front().c_str(), lineLength, MPI_CHAR, &lineChars[0], lineLength, MPI_CHAR, 0, MPI_COMM_WORLD);
//        MPI_Send(fileCopy[processId].c_str(), fileCopy[processId].size(), MPI_CHAR, fileCopy[processId], fileCopy[processId].size(), MPI_CHAR, 0, MPI_COMM_WORLD);



//        MPI_Send(fileCopy[processId].c_str(), fileCopy[processId].size(), MPI_CHAR, processId, READ_INPUT, MPI_COMM_WORLD);
//        MPI_Wait(&requests, &status);
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