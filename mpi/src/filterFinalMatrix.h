#ifndef SERIAL_FILTERFINALMATRIX_H
#define SERIAL_FILTERFINALMATRIX_H

#include <iostream>
#include "computeB.h"

void filterFinalMatrix(double *&A, double *&B,
                       int *&nonZeroUserIndexes,
                       int *&nonZeroItemIndexes,
                       double *&nonZeroElements,
                       double *&L,
                       double *&R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       int *&BV, int &processId);

#endif //SERIAL_FILTERFINALMATRIX_H
