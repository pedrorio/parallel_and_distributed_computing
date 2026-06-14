#ifndef OMP_FILTERFINALMATRIX_H
#define OMP_FILTERFINALMATRIX_H

void filterFinalMatrix(double *&A, double *&B,
                       int *&nonZeroUserIndexes,
                       int *&nonZeroItemIndexes,
                       double *&nonZeroElements,
                       double *&L,
                       double *&R,
                       int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
                       int &numberOfNonZeroElements,
                       int *&BV);


#endif //OMP_FILTERFINALMATRIX_H
