#ifndef SERIAL_FILTERFINALMATRIX_H
#define SERIAL_FILTERFINALMATRIX_H

void filterFinalMatrix(double *&A, double *&B,
                       int *&nonZeroUserIndexes,
                       int *&nonZeroItemIndexes,
                       double *&nonZeroElements,
                       double *&L,
                       double *&R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       int *&BV);


#endif //SERIAL_FILTERFINALMATRIX_H
