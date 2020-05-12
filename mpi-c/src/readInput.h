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

    int numberOfIterations;
    double convergenceCoefficient;
    int numberOfFeatures;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;


    const char space[2] = " ";
    char *token;
//
    int maxDoubleSize = snprintf(NULL, 0, "%d", (int) DBL_MAX);
    int maxLineSize = 3 * maxDoubleSize + 2 + 1;
//    char line[maxLineSize];

    struct fileCopy *fc = createFileCopy(fc, inputFileName);
    struct configuration c;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast((void *) (fc->content), maxLineSize * fc->totalNumberOfLines, MPI_CHAR, ROOT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < 4; i++) {
        int tokenCount = 0;
        char *rest = &(fc->content)[i * maxLineSize];
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



    c.numberOfIterations = numberOfIterations;
    c.convergenceCoefficient = convergenceCoefficient;
    c.numberOfFeatures = numberOfFeatures;
    c.numberOfUsers = numberOfUsers;
    c.numberOfItems = numberOfItems;
    c.numberOfNonZeroElements = numberOfNonZeroElements;

    MatrixA = (double *) malloc(numberOfUsers * numberOfItems * sizeof(double));

    for (int l = 0; l < numberOfUsers; l++) {
        for (int i = 0; i < numberOfItems; i++) {
            MatrixA[l * numberOfItems + i] = (double) 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int k = 4; k < fc->totalNumberOfLines; k++) {
        int tokenCount = 0;
        char *rest = &(fc->content)[k * maxLineSize];

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
//        for (int l = 0; l < numberOfUsers; l++) {
//            for (int i = 0; i < numberOfItems; i++) {
//                printf("%f ", MatrixA[l * numberOfItems + i]);
//                fflush(stdout);
//            }
//            printf("\n");
//            fflush(stdout);
//        }
//    }
    free(A);
    free(MatrixA);

}


#endif //SERIAL_READINPUT_H
