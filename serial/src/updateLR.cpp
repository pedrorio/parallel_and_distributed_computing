//
// Created by Pedro Rio on 19/03/2020.
//

#include <iostream>
#include "updateLR.h"

void
updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B, std::vector<std::vector<double>> &L,
         std::vector<std::vector<double>> &R, std::vector<std::vector<double>> &StoreL,
         std::vector<std::vector<double>> &StoreR,
         int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, int &numberOfNonZeroElements,
         double &convergenceCoefficient) {

//     compute Rkji = 2(Aij - Bij) * -1 * Lik and sum it for all i
//     compute Likj = 2(Aij - Bij) * -1 * Rkj and sum it for all j
//     i => user
//     j => item
//     k => latentFeature

    double itemSum = 0;
    double userSum = 0;

    //     compute dDij/dLikj = Likj = 2(Aij - Bij) * -1 * Rkj and sum for all j
    //     Dij = SDij - a * dDij/dRkji
    //     compute dDij/dRkji = 2(Aij - Bij) * -1 * SLik and sum for all i
    //     Rkj = SRkj - a * dDij/dRkji

    L = StoreL;
    R = StoreR;

    std::vector<std::vector<int>> nonZeroElementIndexes;
    std::vector<int> items;

    //  Likj - ikj sums all j
    //  Rkji - kji sums all i


    for (int user = 0; user < numberOfUsers; user++) {
        for (int item = 0; item < numberOfItems; item++) {
            if (A[user][item] != 0) {
                nonZeroElementIndexes.push_back({user, item});
            }
        }
    }

    for (auto &nonZeroElementIndex: nonZeroElementIndexes) {
        for (int feature = 0; feature < numberOfFeatures; feature++) {
            int user = nonZeroElementIndex[0];
            int item = nonZeroElementIndex[1];

            for (int userNumber = 0; userNumber < numberOfUsers; userNumber++) {
                userSum -= 2 * (A[userNumber][item] - B[userNumber][item]) * StoreL[userNumber][feature];
            }

            for (int itemNumber = 0; itemNumber < numberOfItems; itemNumber++) {
                itemSum -= 2 * (A[user][itemNumber] - B[user][itemNumber]) * StoreR[feature][itemNumber];
            }

//            userSum -= 2 * (A[user][item] - B[user][item]) * StoreL[user][feature];
//            itemSum -= 2 * (A[user][item] - B[user][item]) * StoreR[feature][item];
        }

        for (int feature = 0; feature < numberOfFeatures; feature++) {
            int user = nonZeroElementIndex[0];
            int item = nonZeroElementIndex[1];
            L[user][feature] = StoreL[user][feature] - convergenceCoefficient * itemSum;
            R[feature][item] = StoreR[feature][item] - convergenceCoefficient * userSum;
        }
    }
};