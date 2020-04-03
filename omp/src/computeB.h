#ifndef SERIAL_COMPUTEB_H
#define SERIAL_COMPUTEB_H

#include <vector>

void computeB(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                                          int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, std::vector<std::vector<double>> &B);


#endif //SERIAL_COMPUTEB_H
