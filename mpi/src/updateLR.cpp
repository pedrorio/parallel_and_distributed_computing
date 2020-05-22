#include <iostream>
#include "updateLR.h"

#include "mpi.h"

#define ROOT 0

#define BLOCK_LOW(id, p, n) ((id)*(n)/(p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id)+1,p,n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n) + 1)
#define BLOCK_OWNER(index, p, n) (((p)*((index)+1)-1)/(n))

void updateLR(double *&A,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient,
              int &processId, int &numberOfProcesses) {

    MPI_Barrier(MPI_COMM_WORLD);

    // GLOBAL PREDICTION AND DELTA
    auto *prediction = new double[numberOfNonZeroElements];
    auto *delta = new double[numberOfNonZeroElements];
    for (int i = 0; i < numberOfNonZeroElements; i++) {
        prediction[i] = 0;
        delta[i] = 0;
    }

    // LOCAL PREDICTION, DELTA, BLOCK SIZE AND START INDEX
    int nZStartIndex = BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements);
    int nZBlockSize = BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements);

    auto *sUserI = new int[nZBlockSize];
    auto *sItemI = new int[nZBlockSize];
    auto *sPrediction = new double[nZBlockSize];
    auto *sDelta = new double[nZBlockSize];
    for (int l = 0; l < nZBlockSize; l++) {
        sUserI[l] = nonZeroUserIndexes[nZStartIndex + l];
        sItemI[l] = nonZeroItemIndexes[nZStartIndex + l];
        sPrediction[l] = 0;
        sDelta[l] = 0;
    }

    // GLOBAL COUNTS AND DISPLACEMENTS
    auto *nZCounts = new int[numberOfProcesses];
    auto *nZDisplacements = new int[numberOfProcesses];
    for (int j = 0; j < numberOfProcesses; j++) {
        nZCounts[j] = BLOCK_SIZE(j, numberOfProcesses, numberOfNonZeroElements);
        nZDisplacements[j] = BLOCK_LOW(j, numberOfProcesses, numberOfNonZeroElements);
    }

    for (int l = 0; l < nZBlockSize; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            sPrediction[l] += L[sUserI[l] * numberOfFeatures + k] * R[k * numberOfItems + sItemI[l]];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allgatherv(&sPrediction[0], nZBlockSize, MPI_DOUBLE, &prediction[0], nZCounts,
                   nZDisplacements, MPI_DOUBLE, MPI_COMM_WORLD);

    for (int l = 0; l < nZBlockSize; l++) {
        sDelta[l] = A[sUserI[l] * numberOfItems + sItemI[l]] - sPrediction[l];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allgatherv(&sDelta[0], nZBlockSize, MPI_DOUBLE, &delta[0], nZCounts,
                   nZDisplacements, MPI_DOUBLE, MPI_COMM_WORLD);

    int startIndex = nZDisplacements[processId];
    int blockSize = nZDisplacements[processId];

    int lStartUserIndex = nonZeroUserIndexes[startIndex];
    int lEndUserIndex = nonZeroUserIndexes[startIndex + blockSize];

    int rStartUserIndex = nonZeroItemIndexes[startIndex];
    int rEndUserIndex = nonZeroItemIndexes[startIndex + blockSize];

    int lSize = numberOfUsers * numberOfFeatures;
    auto *sL = new double[lSize];
    for (int i = 0; i < numberOfUsers * numberOfFeatures; i++) {
        sL[i] = sL[i];
    }

    int rSize = numberOfFeatures * numberOfItems;
    auto *sR = new double[rSize];
    for (int i = 0; i < rSize; i++) {
        sR[i] = R[i];
    }

//    auto *lCounts = new int[numberOfProcesses];
//    auto *lDisplacements = new int[numberOfProcesses];
//    for (int j = 0; j < numberOfProcesses; j++) {
//        lCounts[j] = BLOCK_SIZE(j, numberOfProcesses, lSize);
//        lDisplacements[j] = BLOCK_LOW(j, numberOfProcesses, lSize);
//    }

//    auto *rCounts = new int[numberOfProcesses];
//    auto *rDisplacements = new int[numberOfProcesses];
//    for (int j = 0; j < numberOfProcesses; j++) {
//        rCounts[j] = BLOCK_SIZE(j, numberOfProcesses, rSize);
//        rDisplacements[j] = BLOCK_LOW(j, numberOfProcesses, rSize);
//    }

    // SCATTERV
    for (int l = 0; l < nZBlockSize; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            sL[nonZeroUserIndexes[l] * numberOfFeatures + k] +=
                    convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
            sR[k * numberOfItems + nonZeroItemIndexes[l]] +=
                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
        }
    }

    // GATHERV

    delete[] prediction;
    delete[] delta;
    delete[] sPrediction;
    delete[] sDelta;

    delete[] sL;
    delete[] lCounts;
    delete[] lDisplacements;

    delete[] sR;
    delete[] rCounts;
    delete[] rDisplacements;

    delete[] sUserI;
    delete[] sItemI;
    delete[] sL;
    delete[] sR;
    delete[] nZCounts;
    delete[] nZDisplacements;
}