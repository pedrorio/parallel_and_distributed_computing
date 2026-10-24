#ifndef MPIGRID_INITIALLR_H
#define MPIGRID_INITIALLR_H

#include "cell.h"
#include "config.h"

// Deterministic init: every rank advances the full srandom(1) sequence in the serial
// fill order but stores only its own block of L (user-rows) and R (item-columns), so
// the slices hold exactly the serial values. L is uLocal x features, R is
// features x iLocal (both caller-allocated).
void initialLR(const Config &cfg, const Cell &cell, double *L, double *R);

#endif  // MPIGRID_INITIALLR_H
