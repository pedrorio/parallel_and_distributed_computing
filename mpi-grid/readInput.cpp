#include "readInput.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#define ROOT 0

void readInput(const std::string &inputFile, const Grid &g, Config &cfg,
               std::vector<int> &gUser, std::vector<int> &gItem, std::vector<double> &gVal) {
    int header[5] = {0, 0, 0, 0, 0};   // iterations, features, users, items, nonZeros
    double alpha = 0.0;
    int ok = 1;

    if (g.world == ROOT) {
        std::ifstream f(inputFile);
        if (!f) {
            ok = 0;
        } else {
            std::vector<std::string> lines;
            for (std::string l; std::getline(f, l);) lines.push_back(l);
            try {
                if (lines.size() < 4) throw std::runtime_error("missing header");
                header[0] = std::stoi(lines[0]);
                alpha     = std::stod(lines[1]);
                header[1] = std::stoi(lines[2]);
                std::istringstream iss(lines[3]);
                std::vector<std::string> d(std::istream_iterator<std::string>{iss},
                                           std::istream_iterator<std::string>());
                if (d.size() < 3) throw std::runtime_error("missing dimensions");
                header[2] = std::stoi(d[0]);
                header[3] = std::stoi(d[1]);
                header[4] = std::stoi(d[2]);
                if (header[1] <= 0 || header[2] <= 0 || header[3] <= 0 || header[4] < 0 ||
                    (int) lines.size() < 4 + header[4])
                    throw std::runtime_error("inconsistent counts");
                for (int m = 0; m < header[4]; m++) {
                    std::istringstream ls(lines[4 + m]);
                    std::vector<std::string> r(std::istream_iterator<std::string>{ls},
                                               std::istream_iterator<std::string>());
                    if (r.size() < 3) throw std::runtime_error("malformed non-zero");
                    int u = std::stoi(r[0]), it = std::stoi(r[1]);
                    if (u < 0 || u >= header[2] || it < 0 || it >= header[3])
                        throw std::runtime_error("non-zero index out of range");
                    gUser.push_back(u);
                    gItem.push_back(it);
                    gVal.push_back(std::stod(r[2]));
                }
            } catch (const std::exception &) {
                ok = 0;
            }
        }
    }

    MPI_Bcast(&ok, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    if (!ok) {
        if (g.world == ROOT)
            std::cerr << "matFact-grid: could not read a valid instance from '" << inputFile << "'" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Bcast(header, 5, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&alpha, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    cfg.iterations  = header[0];
    cfg.features    = header[1];
    cfg.users       = header[2];
    cfg.items       = header[3];
    cfg.nonZeros    = header[4];
    cfg.convergence = alpha;
}
