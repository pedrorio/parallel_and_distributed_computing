#include <cstring>
#include "readInput.h"

#define READ_INPUT 123
#define NUMBER_OF_LINES 132
#define SEND_MATRIX 132
#define SEND_SIZES 131
#define ROOT 0

#define FIRST_ELEMENT(id, p, n) ((id)*(n)/(p))
#define LAST_ELEMENT(id, p, n) (FIRST_ELEMENT((id)+1,p,n)-1)
#define BLOCK_SIZE(id, p, n) (LAST_ELEMENT(id,p,n)-FIRST_ELEMENT(id,p,n)+1)

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str); // Turn the string into a stream.
    std::string tok;

    while (getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

void readInput(std::string &inputFileName, std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements, int &processId, int &numberOfProcesses) {

    MPI_Request readInputRequest;
    MPI_Status readInputStatus;

    MPI_Request sendSubMatrixRequest;
    MPI_Status sendSubMatrixStatus;

    std::vector<std::string> fileCopy;
    std::vector<std::string> receiveFileLines;
    std::string line;

    int i, k, m;

    int numberOfLines = 0;

    // receive lineSize
    // receive line
    if (processId != ROOT) {
        MPI_Probe(0, READ_INPUT, MPI_COMM_WORLD, &readInputStatus);
        int lineSize;
        MPI_Get_count(&readInputStatus, MPI_CHAR, &lineSize);
        char lineChars[lineSize];
        MPI_Irecv(&lineChars, lineSize, MPI_CHAR, 0, READ_INPUT, MPI_COMM_WORLD, &readInputRequest);
        line = lineChars;

        MPI_Wait(&readInputRequest, &readInputStatus);
    }

    // compute numberOfLines
    // broadcast numberOfLines
    if (processId == ROOT) {
        std::ifstream countFileLines(inputFileName);
        for (numberOfLines = 0; std::getline(countFileLines, line); numberOfLines++) {
            fileCopy.push_back(line);
        };
        countFileLines.close();
    }
    MPI_Bcast(&numberOfLines, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (processId == ROOT) {
        // compute numberOfIterations [not used here]
        // compute numberOfUsers
        // compute numberOfItems
        // compute numberOfNonZeroElements
        // compute convergenceCoefficient [not used here]
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

                    break;
                }
            }
        }

        std::vector<std::string> sendFileLines = std::vector<std::string>(&fileCopy[4], &fileCopy[numberOfLines]);

        // Send string of joined strings
        for (int j = 0; j < numberOfProcesses; j++) {
            std::vector<std::string> words = std::vector<std::string>(
                    &sendFileLines[FIRST_ELEMENT(j, numberOfProcesses, sendFileLines.size())],
                    &sendFileLines[LAST_ELEMENT(j, numberOfProcesses, sendFileLines.size()) + 1]
            );


            // Join lines
            std::string phrase = "";
            for (int m = 0; m < words.size(); m++) {
                phrase += words[m];
                if (m != words.size() - 1) {
                    phrase += ",";
                }
            }

            // Send lines
            if (j == ROOT) {
                line = phrase;
            } else {
                MPI_Isend(phrase.c_str(), phrase.size(), MPI_CHAR, j, READ_INPUT, MPI_COMM_WORLD,
                          &readInputRequest);
            }
        }
    }

    MPI_Bcast(&numberOfUsers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfItems, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

//    std::cout << "process " << processId << " numberOfUsers " << numberOfUsers << std::endl;
//    std::cout << "process " << processId << " numberOfItems " << numberOfItems << std::endl;

    std::vector<std::vector<double>> StoreA(numberOfUsers, std::vector<double>(numberOfItems, 0));

    std::vector<int> userIndexes;
    std::vector<int> itemIndexes;
    std::vector<double> elements;

    // split strings
    std::vector<std::string> lines = split(line, ',');

    for (auto &lineElement: lines) {
        std::vector<std::string> results = split(lineElement, ' ');

        int userIndex = std::stoi(results[0]);
        int itemIndex = std::stoi(results[1]);
        double element = std::stod(results[2]);

        userIndexes.push_back(userIndex);
        itemIndexes.push_back(itemIndex);
        elements.push_back(element);
    }

    for (int n = 0; n < elements.size(); n++) {
//        std::cout << "process " << processId << " has userIndex " << userIndexes[n] << " and itemIndex " << itemIndexes[n] << std::endl;
        StoreA[userIndexes[n]][itemIndexes[n]] = elements[n];
//        std::cout << "Process " << processId << " has StoreA element " << StoreA[userIndexes[n]][itemIndexes[n]] << std::endl;
    }


    int tamanhos[numberOfProcesses];
    int displacements[numberOfProcesses];

    int displacement = (userIndexes[0] * numberOfItems + itemIndexes[0]);
    int tamanho = (userIndexes[userIndexes.size() - 1] * numberOfItems + itemIndexes[itemIndexes.size() - 1]) -
                  (userIndexes[0] * numberOfItems + itemIndexes[0]);

    std::cout << "tamanho is " << tamanho << " in process " << processId << std::endl;

//    std::vector<int> counts(numberOfProcesses, 1);
    // GATHER HERE
    MPI_Allgather(&tamanho, 1, MPI_INT, &tamanhos, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&displacement, 1, MPI_INT, &displacements, 1, MPI_INT, MPI_COMM_WORLD);
//    MPI_Gatherv(tamanho, 1, MPI_INT, &tamanhos, &localTamanhos, &displacements, MPI_INT, ROOT, MPI_COMM_WORLD);


if (processId == ROOT) {
    for (int j = 1; j < numberOfProcesses; j++) {

        int itemIndex = displacements[j] - userIndexes[j] * numberOfItems;
        int userIndex = (displacements[j] - itemIndexes[j]) / numberOfItems;
        std::cout << "Process " << j << "userIndex " << userIndex << std::endl;
        std::cout << "Process " << j << "itemIndex  " << itemIndex << std::endl;

//            std::cout << StoreA[userIndex][itemIndex] << std::endl;
        MPI_Irecv(&StoreA[userIndex][itemIndex], tamanhos[j], MPI_DOUBLE, j, SEND_SIZES, MPI_COMM_WORLD,
                  &sendSubMatrixRequest);
    }

    MPI_Wait(&sendSubMatrixRequest, &sendSubMatrixStatus);
    tamanhos[ROOT] = tamanho;

    for (int p = 0; p < numberOfUsers; p++) {
        for (int j = 0; j < numberOfItems; j++) {
//                std::cout << StoreA[p][j] << std::endl;
        }
    }
}
    if (processId != ROOT) {
        MPI_Isend(&StoreA + userIndexes[0] * numberOfItems + itemIndexes[0], tamanho, MPI_DOUBLE, ROOT, SEND_SIZES,
                  MPI_COMM_WORLD, &sendSubMatrixRequest);
    }



//        A[userIndex][itemIndex] = element;
//        nonZeroElementIndexes[m - 4] = {userIndex, itemIndex};

}
