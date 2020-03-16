//
// Created by Pedro Rio on 14/03/2020.
//

#include "initialB.h"

void initialB(std::vector<std::vector<double>> &B, int &numberOfUsers, int &numberOfItems) {
    B.resize(numberOfUsers);
    for (int i = 0; i < numberOfUsers; i++)
        B[i].resize(numberOfItems, 0);
};

