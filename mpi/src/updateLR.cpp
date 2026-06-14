#include "updateLR.h"

#include "mpi.h"

// Contiguous block partition of n items across p ranks (Quinn's BLOCK_LOW/SIZE,
// as typed inline helpers rather than macros to avoid double-evaluation pitfalls).
static inline int blockLow(int id, int p, int n) { return id * n / p; }
static inline int blockCount(int id, int p, int n) { return blockLow(id + 1, p, n) - blockLow(id, p, n); }

void updateLR(double *A,
              int *nonZeroUserIndexes, int *nonZeroItemIndexes,
              double *L, double *R, double *StoreL, double *StoreR,
              double *dL, double *dR,
              int numberOfUsers, int numberOfItems, int numberOfFeatures,
              int numberOfNonZeroElements, double convergenceCoefficient,
              int processId, int numberOfProcesses) {

    int lSize = numberOfUsers * numberOfFeatures;
    int rSize = numberOfFeatures * numberOfItems;

    // Snapshot the current L and R (identical on every process). Every read in
    // this iteration uses the snapshot, never a partially-updated value.
    for (int i = 0; i < lSize; i++) StoreL[i] = L[i];
    for (int i = 0; i < rSize; i++) StoreR[i] = R[i];

    // This process's gradient contributions start at zero.
    for (int i = 0; i < lSize; i++) dL[i] = 0.0;
    for (int i = 0; i < rSize; i++) dR[i] = 0.0;

    // This process owns one contiguous block of the non-zero list. (When there
    // are more processes than non-zeros, some blocks are empty.)
    int startIndex = blockLow(processId, numberOfProcesses, numberOfNonZeroElements);
    int blockSize = blockCount(processId, numberOfProcesses, numberOfNonZeroElements);

    for (int l = startIndex; l < startIndex + blockSize; l++) {
        int u = nonZeroUserIndexes[l];
        int i = nonZeroItemIndexes[l];

        double prediction = 0.0;
        for (int k = 0; k < numberOfFeatures; k++) {
            prediction += StoreL[u * numberOfFeatures + k] * StoreR[k * numberOfItems + i];
        }
        double delta = A[u * numberOfItems + i] - prediction;

        for (int k = 0; k < numberOfFeatures; k++) {
            dL[u * numberOfFeatures + k] +=
                    convergenceCoefficient * (2 * delta * StoreR[k * numberOfItems + i]);
            dR[k * numberOfItems + i] +=
                    convergenceCoefficient * (2 * delta * StoreL[u * numberOfFeatures + k]);
        }
    }

    // Sum every process's contributions in place: dL and dR become the global
    // increment, identical on every rank.
    MPI_Allreduce(MPI_IN_PLACE, dL, lSize, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, dR, rSize, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    // Apply the global increment to the snapshot.
    for (int i = 0; i < lSize; i++) L[i] = StoreL[i] + dL[i];
    for (int i = 0; i < rSize; i++) R[i] = StoreR[i] + dR[i];
}
