#include <iostream>

#include "matFact.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <instance.in>" << std::endl;
        return 1;
    }
    std::string inputFileName = argv[1];
    matFact(inputFileName);
    return 0;
}