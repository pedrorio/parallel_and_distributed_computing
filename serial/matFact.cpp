#include <vector>
#include "src/readInput.h"
#include "src/writeInitialMatrices.h"
#include "src/initialB.h"
#include "src/initialLR.h"
#include "src/updateB.h"

int main(int argc, char *argv[]) {
    int numberOfIterations;
    int numberOfLatentFeatures;
    double convergenceCoefficient;

    int numberOfUsers;
    int numberOfItems;
    int numberOfNonZeroElements;

    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> R;
    std::vector<std::vector<double>> B;

    std::ifstream inFile(argv[1]);
    readInput(inFile, A,
              numberOfIterations, numberOfLatentFeatures, convergenceCoefficient,
              numberOfUsers, numberOfItems, numberOfNonZeroElements);
    initialB(B, numberOfUsers, numberOfItems);
    initialLR(L, R, numberOfUsers, numberOfItems, numberOfLatentFeatures);
    updateB(B, L, R, numberOfUsers, numberOfItems, numberOfLatentFeatures);

    std::ofstream matFile("./instances/test.mats");
    writeInitialMatrices(matFile, A, L, R, B);

    return 0;
}