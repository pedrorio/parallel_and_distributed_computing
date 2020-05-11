#ifndef SERIAL_READINPUT_H
#define SERIAL_READINPUT_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "mpi.h"
#include "blockAllocation.h"

#define ROOT 0

void readInput(const char inputFileName[], const int processId, const int numberOfProcesses) {

    int numberOfIterations;
    double convergenceCoefficient;
    int numberOfFeatures;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    const char space[2] = " ";
    char *token;

    double *MatrixA;
    char *fileCopy;

    int maxDoubleSize = snprintf(NULL, 0, "%d", (int) DBL_MAX);
    int maxLineSize = 3 * maxDoubleSize + 2 + 1;
    char line[maxLineSize];

    int totalNumberOfLines;

    if (processId == ROOT) {
        totalNumberOfLines = 0;
        FILE *inputFilePointer;

        inputFilePointer = fopen(inputFileName, "r");
        if (inputFilePointer) {
            while (fgets(line, maxLineSize, inputFilePointer) != NULL) {
                totalNumberOfLines++;
            }
            fclose(inputFilePointer);
        }

        fileCopy = (char *) malloc(totalNumberOfLines * maxLineSize * sizeof(char));
    }

    MPI_Bcast(&totalNumberOfLines, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (processId == ROOT) {
        int lineNumber = 0;
        FILE *inputFilePointer;

        inputFilePointer = fopen(inputFileName, "r");
        if (inputFilePointer) {
            while (fgets(line, maxLineSize, inputFilePointer) != NULL) {
                for (int j = 0; j <= maxLineSize; j++) {
                    fileCopy[lineNumber * maxLineSize + j] = '\0';
                }

                for (int j = 0; j < strlen(line); j++) {
                    fileCopy[lineNumber * maxLineSize + j] = line[j];
                }

                for (int j = 0; j < maxLineSize; j++) {
                    printf("%c", fileCopy[lineNumber * maxLineSize + j]);
                }

                lineNumber++;
            }
            fclose(inputFilePointer);
        }

    }
    printf("totalNumberOfLines: %d\n", totalNumberOfLines);

    if (processId != ROOT) {
        fileCopy = (char *) malloc(totalNumberOfLines * maxLineSize * sizeof(char));
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(fileCopy, maxLineSize * totalNumberOfLines, MPI_CHAR, ROOT, MPI_COMM_WORLD);
    for (int i = 0; i < totalNumberOfLines; i++) {
        for (int j = 0; j < maxLineSize; j++) {
            printf("%c", fileCopy[i * maxLineSize + j]);
        }
    }
//
//    MPI_Barrier(MPI_COMM_WORLD);
//
    printf("totalNumberOfLines: %d\n", totalNumberOfLines);
    printf("processId: %d in %d\n", processId, numberOfProcesses);
    fflush(stdout);

    for (int m = 0; m < totalNumberOfLines; m++) {
        for (int j = 0; j < maxLineSize; j++) {
            printf("%c", fileCopy[m * maxLineSize + j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
//
//
    for (int i = 0; i < 4; i++) {
        int tokenCount = 0;
        char *rest = &fileCopy[i * maxLineSize];
        while ((token = strtok_r(rest, space, &rest))) {
//                printf("Token: %s\n", token);

            if (i == 0) {
                numberOfIterations = atoi(token);
//                    printf("numberOfIterations: %d\n", numberOfIterations);
            }

            if (i == 1) {
                convergenceCoefficient = strtod(token, &token);
//                    printf("convergenceCoefficient: %f\n", convergenceCoefficient);
            }

            if (i == 2) {
                numberOfFeatures = atoi(token);
//                    printf("numberOfFeatures: %d\n", numberOfFeatures);
            }

            if (i == 3) {
                if (tokenCount == 0) {
                    numberOfUsers = atoi(token);
//                        printf("numberOfUsers: %d\n", numberOfUsers);
                }

                if (tokenCount == 1) {
                    numberOfItems = atoi(token);
//                        printf("numberOfItems: %d\n", numberOfItems);
                }

                if (tokenCount == 2) {
                    numberOfNonZeroElements = atoi(token);
//                        printf("numberOfNonZeroElements: %d\n", numberOfNonZeroElements);
                }
            }
            tokenCount++;
        }
    }

    MatrixA = (double *) malloc(numberOfUsers * numberOfItems * sizeof(double));

    for (int l = 0; l < numberOfUsers; l++) {
        for (int i = 0; i < numberOfItems; i++) {
            MatrixA[l * numberOfItems + i] = 0.0;
        }
    }

    printf("processId: %d\n", processId);
    fflush(stdout);

    for (int k = 4; k < totalNumberOfLines; k++) {
        int tokenCount = 0;
        char *rest = &fileCopy[k * maxLineSize];

        int userIndex;
        int itemIndex;
        double nonZeroElement;

        while ((token = strtok_r(rest, space, &rest))) {
//                printf("Token: %s\n", token);

            if (tokenCount == 0) {
                userIndex = atoi(token);
//                    printf("userIndex: %d\n", userIndex);
            }

            if (tokenCount == 1) {
                itemIndex = atoi(token);
//                    printf("itemIndex: %d\n", itemIndex);
            }

            if (tokenCount == 2) {
                nonZeroElement = strtod(token, &token);
//                    printf("nonZeroElement: %f\n", nonZeroElement);
            }
            tokenCount++;
        }
        MatrixA[userIndex * numberOfItems + itemIndex] = nonZeroElement;
    }
    free(fileCopy);

    for (int l = 0; l < numberOfUsers; l++) {
        for (int i = 0; i < numberOfItems; i++) {
            printf("%f ", MatrixA[l * numberOfItems + i]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    free(MatrixA);
}


#endif //SERIAL_READINPUT_H
