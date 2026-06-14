#include "initialLR.h"

#include <cstdlib>

#define RAND01 ((double) random() / (double) RAND_MAX)

void initialLR(const Config &cfg, const Cell &cell, double *L, double *R) {
    int F = cfg.features, U = cfg.users, I = cfg.items;

    srandom(1);
    for (int u = 0; u < U; u++)
        for (int k = 0; k < F; k++) {
            double v = RAND01 / (double) F;
            if (u >= cell.userStart && u < cell.userStart + cell.uLocal)
                L[(size_t) (u - cell.userStart) * F + k] = v;
        }
    for (int k = 0; k < F; k++)
        for (int it = 0; it < I; it++) {
            double v = RAND01 / (double) F;
            if (it >= cell.itemStart && it < cell.itemStart + cell.iLocal)
                R[(size_t) k * cell.iLocal + (it - cell.itemStart)] = v;
        }
}
