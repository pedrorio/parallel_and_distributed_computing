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

    std::cout << "startIndex " << startIndex << " [" << processId << "]" << std::endl;
    fflush(stdout);

    std::cout << "blockSize " << blockSize << " [" << processId << "]" << std::endl;
    fflush(stdout);

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

    std::cout << "HERE[" << processId << "]" << std::endl;
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int m = 0; m < blockSize; m++) {
        std::cout << "sPrediction[" << m << "][" << processId << "] = " << sPrediction[m] << std::endl;
        fflush(stdout);
    }

    for (int m = 0; m < numberOfProcesses; m++) {
        std::cout << "counts[" << m << "][" << processId << "] = " << counts[m] << std::endl;
        fflush(stdout);
    }

    for (int m = 0; m < numberOfProcesses; m++) {
        std::cout << "displacements[" << m << "][" << processId << "] = " << displacements[m] << std::endl;
        fflush(stdout);
    }

    MPI_Allgatherv(&sPrediction[0], blockSize, MPI_DOUBLE, &prediction[0], counts,
                   displacements, MPI_DOUBLE, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction[l];
    }

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




//
//#include <iostream>
//#include "updateLR.h"
//
//#include "mpi.h"
//
//#define ROOT 0
//
//#define BLOCK_LOW(id, p, n) ((id)*(n)/(p))
//#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id)+1,p,n) - 1)
//#define BLOCK_SIZE(id, p, n) (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n) + 1)
//#define BLOCK_OWNER(index, p, n) (((p)*((index)+1)-1)/(n))
//
//void updateLR(double *&A,
//              int *&nonZeroUserIndexes,
//              int *&nonZeroItemIndexes,
//              double *&L, double *&R,
//              double *&StoreL, double *&StoreR,
//              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
//              int &numberOfNonZeroElements,
//              double &convergenceCoefficient,
//              int &processId, int &numberOfProcesses) {
//
//    MPI_Barrier(MPI_COMM_WORLD);
//
//    auto *prediction = new double[numberOfNonZeroElements];
//    auto *delta = new double[numberOfNonZeroElements];
//    for (int i = 0; i < numberOfNonZeroElements; i++) {
//        prediction[i] = 0;
//        delta[i] = 0;
//    }
//
//    int startIndex = BLOCK_LOW(processId, numberOfProcesses, numberOfNonZeroElements);
//    int blockSize = BLOCK_SIZE(processId, numberOfProcesses, numberOfNonZeroElements);
//    int endIndex = BLOCK_HIGH(processId, numberOfProcesses, numberOfNonZeroElements);
//
//    std::cout << "startIndex " << startIndex << " [" << processId << "]" << std::endl;
//    fflush(stdout);
//
//    std::cout << "blockSize " << blockSize << " [" << processId << "]" << std::endl;
//    fflush(stdout);
//
//    auto *storeNonZeroUserIndexes = new int[blockSize];
//    auto *storeNonZeroItemIndexes = new int[blockSize];
//    auto *storePrediction = new double[blockSize];
//    auto *storeDelta = new double[blockSize];
//    for (int l = 0; l < blockSize; l++) {
//        storeNonZeroUserIndexes[l] = nonZeroUserIndexes[startIndex + l];
//        storeNonZeroItemIndexes[l] = nonZeroItemIndexes[startIndex + l];
//        storePrediction[l] = 0;
//        storeDelta[l] = 0;
//    }
//
//    auto *receiveCounts = new int[numberOfProcesses];
//    auto *receiveDisplacement = new int[numberOfProcesses];
//    for (int j = 0; j < numberOfProcesses; j++) {
//        receiveCounts[j] = blockSize;
//        receiveDisplacement[j] = startIndex;
//    }
//
//    for (int l = 0; l < blockSize; l++) {
//        for (int k = 0; k < numberOfFeatures; k++) {
//            storePrediction[l] += L[storeNonZeroUserIndexes[l] * numberOfFeatures + k] *
//                                  R[k * numberOfItems + storeNonZeroItemIndexes[l]];
//        }
//    }
//
//    std::cout << "HERE[" << processId << "]" << std::endl;
//    fflush(stdout);
//
//    MPI_Allgatherv(&storePrediction[0], blockSize, MPI_DOUBLE, &prediction[0], receiveCounts,
//                   receiveDisplacement, MPI_DOUBLE, MPI_COMM_WORLD);
//
//
//    for (int l = 0; l < blockSize; l++) {
//        storeDelta[l] = A[storeNonZeroUserIndexes[l] * numberOfItems + storeNonZeroItemIndexes[l]] - storePrediction[l];
//    }
//
//    MPI_Allgatherv(&storeDelta[0], blockSize, MPI_DOUBLE, &delta[0], receiveCounts, receiveDisplacement, MPI_DOUBLE,
//                   MPI_COMM_WORLD);
//
//    for (int l = 0; l < blockSize; l++) {
//        for (int k = 0; k < numberOfFeatures; k++) {
//            L[storeNonZeroUserIndexes[l] * numberOfFeatures + k] +=
//                    convergenceCoefficient * (2 * delta[l] * StoreR[k * numberOfItems + nonZeroItemIndexes[l]]);
//            R[k * numberOfItems + storeNonZeroItemIndexes[l]] +=
//                    convergenceCoefficient * (2 * delta[l] * StoreL[nonZeroUserIndexes[l] * numberOfFeatures + k]);
//        }
//    }
//
//    MPI_Allgatherv(&storeDelta[0], blockSize, MPI_DOUBLE, &delta[0], receiveCounts, receiveDisplacement, MPI_DOUBLE,
//                   MPI_COMM_WORLD);
//
//    delete[] prediction;
//    delete[] delta;
//    delete[] storePrediction;
//    delete[] storeDelta;
//
//    delete[] storeNonZeroUserIndexes;
//    delete[] storeNonZeroItemIndexes;
//    delete[] receiveCounts;
//    delete[] receiveDisplacement;
//}
//
