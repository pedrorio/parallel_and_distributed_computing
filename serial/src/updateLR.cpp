//
// Created by Pedro Rio on 19/03/2020.
//

#include <iostream>
#include "updateLR.h"

void
updateLR(std::vector<std::vector<double>> &A, std::vector<std::vector<int>> &nonZeroElementIndexes,
         std::vector<std::vector<double>> &B, std::vector<std::vector<double>> &L,
         std::vector<std::vector<double>> &R, std::vector<std::vector<double>> &StoreL,
         std::vector<std::vector<double>> &StoreR,
         int &numberOfUsers, int &numberOfItems, int &numberOfFeatures, int &numberOfNonZeroElements,
         double &convergenceCoefficient) {

//     compute Rkji = 2(Aij - Bij) * -1 * Lik and sum it for all i
//     compute Likj = 2(Aij - Bij) * -1 * Rkj and sum it for all j
//     i => user
//     j => item
//     k => latentFeature

    double sumForAllUsersOfTheDerivativesWithRespectToR;
    double sumForAllItemsOfTheDerivativesWithRespectToL;

    //     compute dDij/dLikj = Likj = 2(Aij - Bij) * -1 * Rkj and sum for all j
    //     Dij = SDij - a * dDij/dRkji
    //     compute dDij/dRkji = 2(Aij - Bij) * -1 * SLik and sum for all i
    //     Rkj = SRkj - a * dDij/dRkji

    L = StoreL;
    R = StoreR;

    //  Likj - ikj sums all j
    //  Rkji - kji sums all i

    // for each one of the non zero elements
    for (int nonZeroElementNumber = 0; nonZeroElementNumber < numberOfNonZeroElements; nonZeroElementNumber++) {
        int user = nonZeroElementIndexes[nonZeroElementNumber][0];
        int item = nonZeroElementIndexes[nonZeroElementNumber][1];

        // for each one of the features
        for (int feature = 0; feature < numberOfFeatures; feature++) {
            // consider the sum as zero
            sumForAllUsersOfTheDerivativesWithRespectToR = 0;
            sumForAllItemsOfTheDerivativesWithRespectToL = 0;

            // get the sum for all users of the item, for that feature
            for (int userNumber = 0; userNumber < numberOfUsers; userNumber++)
                sumForAllUsersOfTheDerivativesWithRespectToR +=
                        2 * (A[userNumber][item] - B[userNumber][item]) * (-1 * StoreL[userNumber][feature]);

            // get the sum for all items of the user, for that feature
            for (int itemNumber = 0; itemNumber < numberOfItems; itemNumber++)
                sumForAllItemsOfTheDerivativesWithRespectToL +=
                        2 * (A[user][itemNumber] - B[user][itemNumber]) * (-1 * StoreR[feature][itemNumber]);

            // make the adjustment
            L[user][feature] -= convergenceCoefficient * sumForAllItemsOfTheDerivativesWithRespectToL;
            R[feature][item] -= convergenceCoefficient * sumForAllUsersOfTheDerivativesWithRespectToR;
        }
    }

};