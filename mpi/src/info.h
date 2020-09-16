#ifndef MPI_INFO_H
#define MPI_INFO_H

#include <mpi.h>

typedef struct {
    int numberOfProcesses; /* Total number of processes */
    MPI_Comm grid_comm; /* Communicator for entire grid */
    MPI_Comm row_comm; /* Communicator for my row */
    MPI_Comm col_comm; /* Communicator for my col */
    int rowId; /* My row number */
    int colId; /* My column number */
    int processId; /* My rank in the grid comm */
} GRID_INFO_T;

typedef struct {
    int numberOfLines;
    int numberOfIterations;
    int numberOfFeatures;
    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;
    double convergenceCoefficient;
} CONFIG_INFO_T;

#endif //MPI_INFO_H
