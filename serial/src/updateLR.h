#ifndef SERIAL_UPDATELR_H
#define SERIAL_UPDATELR_H

void updateLR(double *&A, int *&nonZeroUserIndexes,
              int *&nonZeroItemIndexes,
              double *&L, double *&R,
              double *&StoreL, double *&StoreR,
              int &numberOfUsers, int &numberOfItems, int &numberOfFeatures,
              int &numberOfNonZeroElements,
              double &convergenceCoefficient);

#endif //SERIAL_UPDATELR_H
