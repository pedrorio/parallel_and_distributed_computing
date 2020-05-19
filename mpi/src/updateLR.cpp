#include <iostream>
#include "updateLR.h"
#include "mpi.h"

void updateLR(double *&A,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient) {


    auto *prediction = new double[numberOfNonZeroElements];
    auto *delta = new double[numberOfNonZeroElements];

//    MPI_Barrier(MPI_COMM_WORLD);
////    printf("[Here][%d]\n", processId);
//    printf("[Here]\n");
//    fflush(stdout);
//    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < numberOfNonZeroElements; i++) {
        prediction[i] = 0;
        delta[i] = 0;
    }

//    MPI_Barrier(MPI_COMM_WORLD);
//    std::cout << "HELLO" << std::endl;

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction[l] +=
                    L[nonZeroUserIndexes[l] * numberOfFeatures + k] * R[k * numberOfItems + nonZeroItemIndexes[l]];
        }
    }

//    MPI_Barrier(MPI_COMM_WORLD);

    for (int l = 0; l < numberOfNonZeroElements; l++) {
        delta[l] = A[nonZeroUserIndexes[l] * numberOfItems + nonZeroItemIndexes[l]] - prediction[l];
    }

//    MPI_Barrier(MPI_COMM_WORLD);

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
}

