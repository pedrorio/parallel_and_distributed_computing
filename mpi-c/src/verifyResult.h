#ifndef OMP_VERIFYRESULT_H
#define OMP_VERIFYRESULT_H

#include <vector>

#include <istream>
#include <fstream>
#include "verifyResult.h"

int verifyResult(char *fileName[], int *BV[]) {
    int realValues[];

    std::ifstream inFile(fileName);
    int element;
    for (std::string line; std::getline(inFile, line);) {
        if (line.empty()) {
            continue;
        }
        element = std::stoi(line);
        realValues.push_back(element);
    }
    inFile.close();

    int numberOfFalses = 0;
    for (int i = 0; i < realValues.size(); i++) {
        if (BV[i] != realValues[i]) {
            numberOfFalses++;
        };
    }
    return numberOfFalses;
};

#endif //OMP_VERIFYRESULT_H