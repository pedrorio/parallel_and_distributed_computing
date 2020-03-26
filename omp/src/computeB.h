//
// Created by Pedro Rio on 26/03/2020.
//

#ifndef SERIAL_COMPUTEB_H
#define SERIAL_COMPUTEB_H

#include <vector>

std::vector<std::vector<double>> computeB(std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                                          int numberOfUsers, int numberOfItems, int numberOfFeatures);


#endif //SERIAL_COMPUTEB_H
