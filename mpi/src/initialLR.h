#ifndef MPI_INITIALLR_H
#define MPI_INITIALLR_H

#include <cstdlib>

void initialLR(double *&L, double *&R,
               int &numberOfUsers, int &numberOfItems, int &numberOfFeatures);

#endif //MPI_INITIALLR_H
