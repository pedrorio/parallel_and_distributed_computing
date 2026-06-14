#ifndef MPIGRID_CONFIG_H
#define MPIGRID_CONFIG_H

// Problem configuration, parsed on the root and broadcast to every rank.
struct Config {
    int iterations;
    int features;
    int users;
    int items;
    int nonZeros;
    double convergence;
};

#endif  // MPIGRID_CONFIG_H
