#ifndef MPI_STRUCTURES_H
#define MPI_STRUCTURES_H

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "mpi.h"
#include "blockAllocation.h"
//
//struct fileCopy {
//    int totalNumberOfLines;
//    char content[];
//};
//
//struct fileCopy *createFileCopy(struct fileCopy *fc, const char inputFileName[]) {
//
//    int maxDoubleSize = snprintf(NULL, 0, "%d", (int) DBL_MAX);
//    int maxLineSize = 3 * maxDoubleSize + 2 + 1;
//    char line[maxLineSize];
//
//    int totalNumberOfLines = 0;
//    FILE *inputFile;
//
//    inputFile = fopen(inputFileName, "r");
//    if (inputFile) {
//        while (fgets(line, maxLineSize, inputFile) != NULL) {
//            totalNumberOfLines++;
//        }
//        fclose(inputFile);
//    }
//
//    fc = malloc(sizeof *fc + sizeof(int) + totalNumberOfLines * maxLineSize * sizeof(char));
//
//    fc->totalNumberOfLines = totalNumberOfLines;
//
//    int lineNumber = 0;
//    inputFile = fopen(inputFileName, "r");
//    if (inputFile) {
//        while (fgets(line, maxLineSize, inputFile) != NULL) {
//            for (int j = 0; j <= maxLineSize; j++) {
//                fc->content[lineNumber * maxLineSize + j] = '\0';
//            }
//
//            for (int j = 0; j < strlen(line); j++) {
//                fc->content[lineNumber * maxLineSize + j] = line[j];
////                printf("%c", line[j]);
////                fflush(stdout);
//            }
//            lineNumber++;
//        }
//        fclose(inputFile);
//    }
//
//    return fc;
//}

typedef struct {
    int numberOfIterations;
    double convergenceCoefficient;
    int numberOfFeatures;
    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;
} ConfigurationData;


ConfigurationData *createConfigurationData(ConfigurationData *confData,
                                           int numberOfIterations,
                                           double convergenceCoefficient,
                                           int numberOfFeatures,
                                           int numberOfUsers,
                                           int numberOfItems,
                                           int numberOfNonZeroElements
) {
    confData = malloc(sizeof *confData + 5 * sizeof(int) + sizeof(double));

    confData->numberOfIterations = numberOfIterations;
    confData->convergenceCoefficient = convergenceCoefficient;
    confData->numberOfFeatures = numberOfFeatures;
    confData->numberOfUsers = numberOfUsers;
    confData->numberOfItems = numberOfItems;
    confData->numberOfNonZeroElements = numberOfNonZeroElements;
    return confData;
}

#endif //MPI_STRUCTURES_H
