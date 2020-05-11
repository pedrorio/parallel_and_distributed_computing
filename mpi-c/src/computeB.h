#ifndef SERIAL_COMPUTEB_H
#define SERIAL_COMPUTEB_H

void computeB(int &L[][], int &R[][], int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, double &B[][]) {

    int i, j, k;

    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            for (int k = 0; k < numberOfFeatures; k++) {
                B[i][j] += L[i][k] * R[k][j];
            }
        }
    };
}

#endif //SERIAL_COMPUTEB_H
