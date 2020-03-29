#include <istream>
#include <fstream>
#include <iostream>
#include "verifyResult.h"

void verifyResult(std::string &fileName, std::vector<int> &BV) {
    std::vector<int> realValues;

    std::ifstream inFile(fileName);
    int element;
    for (std::string line; std::getline(inFile, line);) {
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
    std::cout << numberOfFalses << std::endl;
};