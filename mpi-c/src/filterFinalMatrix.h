#ifndef SERIAL_FILTERFINALMATRIX_H
#define SERIAL_FILTERFINALMATRIX_H
#include "computeB.h"

int max_index(double *a, int n) {
    if (n <= 0) return -1;
    int i, max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max) {
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

void filterFinalMatrix(double &A[][], double &B[][],
                       int &nonZeroElementIndexes[][2],
                       double &L[][], double &R[][],
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       int &BV[]) {

    int i, j, l;
    {
        computeB(L, R, numberOfUsers, numberOfItems, numberOfFeatures, B);

        for (int l = 0; l < numberOfNonZeroElements; l++) {
            B[nonZeroElementIndexes[l][0]][nonZeroElementIndexes[l][1]] = 0;
        }

        for (int i = 0; i < numberOfUsers; i++) {
            BV[i] = max_index(*B[i], numberOfItems)
            printf("[%d] Default case in send send_recv_centers_of_mass\n", BV[i]);
        };
    };
};

#endif //SERIAL_FILTERFINALMATRIX_H
