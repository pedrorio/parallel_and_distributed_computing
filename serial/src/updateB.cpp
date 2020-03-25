//
// Created by Pedro Rio on 14/03/2020.
//

#include <iostream>
#include "updateB.h"

void updateB(std::vector<std::vector<double>> &B, std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
        int &numberOfUsers, int &numberOfItems, int &numberOfFeatures) {
    for (int user = 0; user < numberOfUsers; user++) {
        for (int item = 0; item < numberOfItems; item++) {
            for (int latentFeature = 0; latentFeature < numberOfFeatures; latentFeature++) {
                B[user][item] += L[user][latentFeature] * R[latentFeature][item];
            }
        }
    }
};