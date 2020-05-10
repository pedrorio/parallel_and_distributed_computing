#include <cstring>
#include "readInput.h"
#include <float.h>

#define READ_INPUT 123
#define NUMBER_OF_LINES 132

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

void readInput(const char inputFileName[], std::vector<std::vector<double>> &A,
               std::vector<std::vector<int>> &nonZeroElementIndexes, int &numberOfIterations,
               int &numberOfFeatures, double &convergenceCoefficient, int &numberOfUsers, int &numberOfItems,
               int &numberOfNonZeroElements, int &processId, int &numberOfProcesses) {

//    MPI_Request readInputRequest;
//    MPI_Status readInputStatus;
//
//    MPI_Request numberOfLinesRequest;
//    MPI_Status numberOfLinesStatus;

//    std::vector<std::string> fileCopy;

//    int i, k, m;
//    double StoreA;
//
    int maxDoubleSize = snprintf(NULL, 0, "%d", DBL_MAX);
    int maxLineSize = 3 * maxDoubleSize + 2;

    char line[maxLineSize];

    FILE *inputFilePointer;

    int totalNumberOfLines = 0;
    inputFilePointer = fopen(inputFileName, "r");
    if (inputFilePointer) {
        while (fgets(line, maxLineSize, inputFilePointer) != NULL) {
            totalNumberOfLines++;
        }
        fclose(inputFilePointer);
    }

    auto fileCopy = (char *) malloc((maxLineSize + 1) * totalNumberOfLines);
//     A = twoDDoubleArray;
//    auto *twoDDoubleArray = (double *)malloc(numberOfLines - 4);

    char *token;
    const char space[2] = " ";

    int numberOfLines = 0;
    inputFilePointer = fopen(inputFileName, "r");
    if (inputFilePointer) {
        while (fgets(line, maxLineSize, inputFilePointer) != NULL) {
            printf("%s", line);
            for (int j = 0; j < strlen(line); j++) {
                fileCopy[numberOfLines * (maxLineSize + 1) + j] = line[j];
            }
            fileCopy[numberOfLines * (maxLineSize + 1) + strlen(line)] = '\0';
            numberOfLines++;

            strtok(fileCopy, space);

        }
        fclose(inputFilePointer);
    }



    // BEGIN
//        if (processId != 0) {
////        MPI_Irecv(&numberOfLines, 1, MPI_INT, 0, NUMBER_OF_LINES, MPI_COMM_WORLD, &numberOfLinesRequest);
////        MPI_Wait(&numberOfLinesRequest, &numberOfLinesStatus);
//
//            MPI_Probe(0, READ_INPUT, MPI_COMM_WORLD, &readInputStatus);
//            int lineSize;
//            MPI_Get_count(&readInputStatus, MPI_CHAR, &lineSize);
//            char lineChars[lineSize];
//            MPI_Irecv(&lineChars, lineSize, MPI_CHAR, 0, READ_INPUT, MPI_COMM_WORLD, &readInputRequest);
//            line = std::string(lineChars);
//
//            MPI_Wait(&readInputRequest, &readInputStatus);
//        }
//
//        if (processId == 0) {
//
//            std::ifstream countFileLines(inputFileName);
//            for (numberOfLines = 0; std::getline(countFileLines, line); numberOfLines++) {
//                fileCopy.push_back(line);
//            };
//            countFileLines.close();
//
//            MPI_Bcast(&numberOfLines, 1, MPI_INT, 0, MPI_COMM_WORLD);
////        MPI_Bcast(&globalUserIndexes, numberOfLines - 4, MPI_INT, 0, MPI_COMM_WORLD);
////        MPI_Bcast(&globalItemIndexes, numberOfLines - 4, MPI_INT, 0, MPI_COMM_WORLD);
////        MPI_Bcast(&globalValues, numberOfLines - 4, MPI_INT, 0, MPI_COMM_WORLD);
////        MPI_Isend(&numberOfLines, 1, MPI_INT, 1, NUMBER_OF_LINES, MPI_COMM_WORLD, &numberOfLinesRequest);
//
//// https://cseweb.ucsd.edu/classes/sp99/cse160/programming/packing/packing.html
//
//// compute number of users
//// compute number of items
//            for (int k = 0; k < 4; k++) {
//                line = fileCopy[k];
//                switch (k) {
//                    case 0: {
//                        numberOfIterations = std::stoi(line);
//                        break;
//                    }
//                    case 1: {
//                        convergenceCoefficient = std::stod(line);
//                        break;
//                    }
//                    case 2: {
//                        numberOfFeatures = std::stoi(line);
//                        break;
//                    }
//                    case 3: {
//                        std::istringstream iss(line);
//
//                        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
//                                                         std::istream_iterator<std::string>());
//                        numberOfUsers = std::stoi(results[0]);
//                        numberOfItems = std::stoi(results[1]);
//                        numberOfNonZeroElements = std::stoi(results[2]);
//
//                        std::vector<std::vector<int>> StoreNonZeroElementIndexes(numberOfNonZeroElements,
//                                                                                 std::vector<int>(2));
//                        nonZeroElementIndexes = StoreNonZeroElementIndexes;
//
//                        std::vector<std::vector<double>> StoreA(numberOfUsers, std::vector<double>(numberOfItems, 0));
//                        A = StoreA;
//                        break;
//                    }
//                }
//            }
//
//            double StoreA[numberOfUsers][numberOfItems];
//            MPI_Bcast(&numberOfUsers, 1, MPI_INT, 0, MPI_COMM_WORLD);
//            MPI_Bcast(&numberOfItems, 1, MPI_INT, 0, MPI_COMM_WORLD);
//            MPI_Bcast(&StoreA, numberOfUsers * numberOfItems, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//
//
//            std::vector<std::string> fileLines = std::vector<std::string>(&fileCopy[4], &fileCopy[numberOfLines]);
//
//            // use scatterv
//            for (int j = 0; j < numberOfProcesses; j++) {
//                std::vector<std::string> processo = std::vector<std::string>(
//                        &fileLines[FIRST_ELEMENT(j, numberOfProcesses, fileLines.size())],
//                        &fileLines[LAST_ELEMENT(j, numberOfProcesses, fileLines.size()) + 1]);
//
//                std::string frases = "";
//                for (int m = 0; m < processo.size(); m++) {
//                    frases += processo[m];
//                    if (m != processo.size() - 1) {
//                        frases += ",";
//                    }
//                }
//
//                if (j == 0) {
//                    line = frases;
//                } else {
//                    MPI_Isend(frases.c_str(), frases.size(), MPI_CHAR, j, READ_INPUT, MPI_COMM_WORLD,
//                              &readInputRequest);
//                }
//            }
//        }
//
//        int globalUserIndexes[numberOfLines - 4];
//        int globalItemIndexes[numberOfLines - 4];
//        int globalValues[numberOfLines - 4];
//
//        std::vector<int> userIndexes;
//        std::vector<int> itemIndexes;
//        std::vector<double> elements;
//
//        // split strings
//        std::vector<std::string> lines = split(line, ',');
//
//        for (auto &lineElement: lines) {
//            std::cout << lineElement << std::endl;
//            std::vector<std::string> results = split(lineElement, ' ');
//
//            int userIndex = std::stoi(results[0]);
//            int itemIndex = std::stoi(results[1]);
//            double element = std::stod(results[2]);
//
//            std::cout << userIndex << std::endl;
//
//            userIndexes.push_back(userIndex);
//            itemIndexes.push_back(itemIndex);
//            elements.push_back(element);
//        }
//
//        int elementsPerProcess[numberOfProcesses];
//        for (int l = 0; l < numberOfProcesses; l++) {
//            elementsPerProcess[l] = BLOCK_SIZE(l, numberOfProcesses, numberOfLines - 4);
//        }
//
//        // get a copy of the matrix subsection per process from the process first non zero element to the process last zero element
////    gatherv the contiguous sections into the final matrix
//
////    (users,items, maxUsers, maxItems) => users * maxItems + items
//
//
//
//
////    MPI_Gatherv(&elements, elements.size(), MPI_INT, &StoreA, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
//        MPI_Gather(&userIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, 0,
//                   MPI_COMM_WORLD);
//        MPI_Gather(&itemIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, 0,
//                   MPI_COMM_WORLD);
//        MPI_Gather(&userIndexes, userIndexes.size(), MPI_INT, &globalUserIndexes, numberOfLines - 4, MPI_INT, 0,
//                   MPI_COMM_WORLD);
// END

//        A[userIndex][itemIndex] = element;
//        nonZeroElementIndexes[m - 4] = {userIndex, itemIndex};
//    }


    free(fileCopy);
}