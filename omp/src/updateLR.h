#ifndef OMP_UPDATELR_H
#define OMP_UPDATELR_H

void updateLR(double *&A,
              double *&delta,
              int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient);

#endif //OMP_UPDATELR_H
