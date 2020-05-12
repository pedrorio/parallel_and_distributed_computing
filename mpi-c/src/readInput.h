#ifndef SERIAL_READINPUT_H
#define SERIAL_READINPUT_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "mpi.h"
#include "blockAllocation.h"
#include "structures.h"

#define ROOT 0

void readInput(const char inputFileName[], const int processId, const int numberOfProcesses) {

    double *MatrixA;
    int *nonZeroElementIndexes;

    // returned in the struct
    int numberOfIterations;
    double convergenceCoefficient;
    int numberOfFeatures;

    // used in this function
    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    const char space[2] = " ";
    char *token;

    int maxDoubleSize = snprintf(NULL, 0, "%d", (int) DBL_MAX);
    int maxLineSize = 3 * maxDoubleSize + 2 + 1;
    char line[maxLineSize];

    int totalNumberOfLines = 0;
    FILE *inputFile;

    inputFile = fopen(inputFileName, "r");
    if (inputFile) {
        while (fgets(line, maxLineSize, inputFile) != NULL) {
            totalNumberOfLines++;
        }
        fclose(inputFile);
    }

    char *fileCopy = malloc(totalNumberOfLines * maxLineSize * sizeof(char));

    int lineNumber = 0;
    inputFile = fopen(inputFileName, "r");
    if (inputFile) {
        while (fgets(line, maxLineSize, inputFile) != NULL) {
            for (int j = 0; j <= maxLineSize; j++) {
                fileCopy[lineNumber * maxLineSize + j] = '\0';
            }

            for (int j = 0; j < strlen(line); j++) {
                fileCopy[lineNumber * maxLineSize + j] = line[j];
//                printf("%c", line[j]);
//                fflush(stdout);
            }
            lineNumber++;
        }
        fclose(inputFile);
    }

    struct configuration conf;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast((void *) (fileCopy), maxLineSize * totalNumberOfLines, MPI_CHAR, ROOT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < 4; i++) {
        int tokenCount = 0;
        char *rest = &fileCopy[i * maxLineSize];
        while ((token = strtok_r(rest, space, &rest))) {
            switch (i) {
                case 0:
                    numberOfIterations = atoi(token);
                    break;
                case 1:
                    convergenceCoefficient = strtod(token, &token);
                    break;
                case 2:
                    numberOfFeatures = atoi(token);
                    break;
                case 3:
                    switch (tokenCount) {
                        case 0:
                            numberOfUsers = atoi(token);
                            break;
                        case 1:
                            numberOfItems = atoi(token);
                            break;
                        case 2:
                            numberOfNonZeroElements = atoi(token);
                            break;
                    }
                    break;
            }
            tokenCount++;
        }
    }

    MatrixA = (double *) malloc(numberOfUsers * numberOfItems * sizeof(double));

    for (int l = 0; l < numberOfUsers; l++) {
        for (int i = 0; i < numberOfItems; i++) {
            MatrixA[l * numberOfItems + i] = (double) 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int k = 4; k < totalNumberOfLines; k++) {
        int tokenCount = 0;
        char *rest = &fileCopy[k * maxLineSize];

        int userIndex;
        int itemIndex;
        double nonZeroElement;

        while ((token = strtok_r(rest, space, &rest))) {
            switch (tokenCount) {
                case 0:
                    userIndex = atoi(token);
                    break;
                case 1:
                    itemIndex = atoi(token);
                    break;
                case 2:
                    nonZeroElement = strtod(token, &token);
                    break;
            }
            tokenCount++;
        }
        MatrixA[userIndex * numberOfItems + itemIndex] = nonZeroElement;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double *A = (double *) malloc(numberOfUsers * numberOfItems * sizeof(double));

    MPI_Reduce(MatrixA, A, numberOfUsers * numberOfItems, MPI_DOUBLE, MPI_MAX, ROOT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

//    if (processId == ROOT) {
//        for (int l = 0; l < conf.numberOfUsers; l++) {
//            for (int i = 0; i < conf.numberOfItems; i++) {
//                printf("%f ", MatrixA[l * conf.numberOfItems + i]);
//                fflush(stdout);
//            }
//            printf("\n");
//            fflush(stdout);
//        }
//    }
    free(A);
    free(MatrixA);
    free(fileCopy);

}


#endif //SERIAL_READINPUT_H
