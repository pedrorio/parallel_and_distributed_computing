#ifndef SERIAL_INITIALLR_H
#define SERIAL_INITIALLR_H


#include <vector>
#include <algorithm>

void initialLR(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
               int &numberOfUsers, int &numberOfItems, int &numberOfFeatures);


#endif //SERIAL_INITIALLR_H
