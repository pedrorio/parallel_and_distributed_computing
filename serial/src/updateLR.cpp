//
// Created by Pedro Rio on 19/03/2020.
//

#include <iostream>
#include "updateLR.h"

void
updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B, std::vector<std::vector<double>> &L,
         std::vector<std::vector<double>> &R, std::vector<std::vector<double>> &StoreL,
         std::vector<std::vector<double>> &StoreR,
         int &numberOfUsers, int &numberOfItems, int &numberOfLatentFeatures, int &numberOfNonZeroElements,
         double &convergenceCoefficient) {

//     compute -2(Aij - Bij) * Mij
//     i => user
//     j => item
//     k => latentFeature

    double itemSum = 0;
    double userSum = 0;

    std::vector<std::vector<double>> D(numberOfUsers, std::vector<double>(numberOfItems, 0));
    std::vector<std::vector<double>> DL(numberOfUsers, std::vector<double>(numberOfLatentFeatures, 0));
    std::vector<std::vector<double>> DR(numberOfLatentFeatures, std::vector<double>(numberOfItems, 0));

    for (int user = 0; user < numberOfUsers; user++) {
        for (int item = 0; item < numberOfItems; item++) {
            if (A[user][item] == 0) continue;
            D[user][item] = -2 * (A[user][item] - B[user][item]);
            for (int latentFeature = 0; latentFeature < numberOfLatentFeatures; latentFeature++) {
                // sum the items
                DL[user][latentFeature] = D[user][item] * StoreR[latentFeature][item];
                // sum the users
                DR[latentFeature][item] = D[user][item] * StoreL[user][latentFeature];
            }
        }
    }

    for (int latentFeature = 0; latentFeature < numberOfLatentFeatures; latentFeature++) {
        for (int user = 0; user < numberOfUsers; user++) {
            // sum the items
            itemSum = 0;
            for (int item = 0; item < numberOfItems; item++) {
                if (A[user][item] == 0) {
                    break;
                }
                itemSum += DL[user][latentFeature];
            }
            L[user][latentFeature] = StoreL[user][latentFeature] - convergenceCoefficient * itemSum;
        }

        for (int item = 0; item < numberOfItems; item++) {
            userSum = 0;
            for (int user = 0; user < numberOfUsers; user++) {
                if (A[user][item] == 0) {
                    break;
                }
                userSum += DR[latentFeature][item];
            }
            R[latentFeature][item] = StoreR[latentFeature][item] - convergenceCoefficient * userSum;
        }
    }
};