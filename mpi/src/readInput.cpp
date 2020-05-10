#include <cstring>
#include "readInput.h"

#define READ_INPUT 123
#define NUMBER_OF_LINES 132
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

    std::vector<std::string> fileCopy;
    std::vector<std::string> receiveFileLines;
    std::string line;


    int i, k, m;
    double StoreA;

    int numberOfLines;


    // receive lineSize
    // receive line
    if (processId != ROOT) {
        MPI_Probe(0, READ_INPUT, MPI_COMM_WORLD, &readInputStatus);
        int lineSize;
        MPI_Get_count(&readInputStatus, MPI_CHAR, &lineSize);
        char lineChars[lineSize];
        MPI_Irecv(&lineChars, lineSize, MPI_CHAR, 0, READ_INPUT, MPI_COMM_WORLD, &readInputRequest);
        line = lineChars;

        std::cout << "Process " << processId << " receives line " << line << std::endl;

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
        MPI_Bcast(&numberOfLines, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

        // compute numberOfIterations [not used here]
        // compute numberOfUsers
        // compute numberOfItems
        // compute numberOfNonZeroElements
        // compute convergenceCoefficient [not used here]
        // initialize A
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

        double StoreA[numberOfUsers][numberOfItems];
        MPI_Bcast(&numberOfUsers, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        MPI_Bcast(&numberOfItems, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        MPI_Bcast(&numberOfNonZeroElements, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        MPI_Bcast(&StoreA, numberOfUsers * numberOfItems, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

        std::vector<std::string> sendFileLines = std::vector<std::string>(&fileCopy[4], &fileCopy[numberOfLines]);

        // Send string of joined strings
        for (int j = 0; j < numberOfProcesses; j++) {
            std::vector<std::string> words = std::vector<std::string>(
                    &sendFileLines[FIRST_ELEMENT(j, numberOfProcesses, sendFileLines.size())],
                    &sendFileLines[LAST_ELEMENT(j, numberOfProcesses, sendFileLines.size()) + 1]);

            for (int l = 0; l < words.size(); l++) {
                std::cout << "Process " << j << " sends word " << l << std::endl;
            }

            std::string phrase = "";
            for (int m = 0; m < words.size(); m++) {
                phrase += words[m];
                if (m != words.size() - 1) {
                    phrase += ",";
                }
            }

            std::cout << "Process " << j << " sends phrase " << phrase << std::endl;

            if (j == 0) {
                line = phrase;
            } else {
                MPI_Isend(phrase.c_str(), phrase.size(), MPI_CHAR, j, READ_INPUT, MPI_COMM_WORLD,
                          &readInputRequest);
            }
        }
    }


    int globalUserIndexes[numberOfLines - 4];
    int globalItemIndexes[numberOfLines - 4];
    int globalValues[numberOfLines - 4];

    std::vector<int> userIndexes;
    std::vector<int> itemIndexes;
    std::vector<double> elements;

    // split strings
    std::vector<std::string> lines = split(line, ',');

    for (auto &lineElement: lines) {
        std::cout << lineElement << std::endl;
        std::vector<std::string> results = split(lineElement, ' ');

        int userIndex = std::stoi(results[0]);
        int itemIndex = std::stoi(results[1]);
        double element = std::stod(results[2]);

        std::cout << userIndex << std::endl;

        userIndexes.push_back(userIndex);
        itemIndexes.push_back(itemIndex);
        elements.push_back(element);
    }

    int elementsPerProcess[numberOfProcesses];
    for (int l = 0; l < numberOfProcesses; l++) {
        elementsPerProcess[l] = BLOCK_SIZE(l, numberOfProcesses, numberOfLines - 4);
    }

    // get a copy of the matrix subsection per process from the process first non zero element to the process last zero element
//    gatherv the contiguous sections into the final matrix

//    (users,items, maxUsers, maxItems) => users * maxItems + items


//    MPI_Gatherv(&elements, elements.size(), MPI_INT, &StoreA, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&userIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, ROOT,
                   MPI_COMM_WORLD);
        MPI_Gather(&itemIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, ROOT,
                   MPI_COMM_WORLD);
        MPI_Gather(&userIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, ROOT,
                   MPI_COMM_WORLD);

//        A[userIndex][itemIndex] = element;
//        nonZeroElementIndexes[m - 4] = {userIndex, itemIndex};
}

