//
// Created by Pedro Rio on 14/03/2020.
//

#ifndef SERIAL_UPDATEB_H
#define SERIAL_UPDATEB_H

#include <vector>

void updateB(std::vector<std::vector<double>> &B, std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
        int &numberOfUsers, int &numberOfItems, int &numberOfFeatures);

#endif //SERIAL_UPDATEB_H
