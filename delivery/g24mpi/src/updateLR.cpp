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

    auto *prediction = new double[numberOfNonZeroElements];
    auto *delta = new double[numberOfNonZeroElements];
    for (int i = 0; i < numberOfNonZeroElements; i++) {
        prediction[i] = 0;
        delta[i] = 0;
    }

    int startIndex = BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements);
    int blockSize = BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements);
    int endIndex = BLOCK_HIGH(processId, numberOfProcesses, numberOfNonZeroElements);

    auto *sUserI = new int[blockSize];
    auto *sItemI = new int[blockSize];
    auto *sPrediction = new double[blockSize];
    auto *sDelta = new double[blockSize];
    for (int l = 0; l < blockSize; l++) {
        sUserI[l] = nonZeroUserIndexes[startIndex + l];
        sItemI[l] = nonZeroItemIndexes[startIndex + l];
        sPrediction[l] = 0;
        sDelta[l] = 0;
    }

    auto *counts = new int[numberOfProcesses];
    auto *displacements = new int[numberOfProcesses];
    for (int j = 0; j < numberOfProcesses; j++) {
        counts[j] = BLOCK_SIZE(j, numberOfProcesses, numberOfNonZeroElements);
        displacements[j] = BLOCK_LOW(j, numberOfProcesses, numberOfNonZeroElements);
    }

    for (int l = 0; l < blockSize; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            sPrediction[l] += L[sUserI[l] * numberOfFeatures + k] * R[k * numberOfItems + sItemI[l]];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allgatherv(&sPrediction[0], blockSize, MPI_DOUBLE, &prediction[0], counts,
                   displacements, MPI_DOUBLE, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int l = 0; l < blockSize; l++) {
        sDelta[l] = A[sUserI[l] * numberOfItems + sItemI[l]] - sPrediction[l];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allgatherv(&sDelta[0], blockSize, MPI_DOUBLE, &delta[0], counts,
                   displacements, MPI_DOUBLE, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            L[nonZeroUserIndexes[l] * numberOfFeatures + k] +=
                    convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
            R[k * numberOfItems + nonZeroItemIndexes[l]] +=
                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
        }
    }

    delete[] prediction;
    delete[] delta;
    delete[] sPrediction;
    delete[] sDelta;

    delete[] sUserI;
    delete[] sItemI;
    delete[] counts;
    delete[] displacements;
}