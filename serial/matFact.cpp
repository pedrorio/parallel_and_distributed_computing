#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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

//numberOfIterations
//convergenceCoefficient
//numberOfLatentFeatures
//numberOfItems numberOfUsers numberOfNonZeroElements

#include <stdlib.h>

#define RAND01 ((double)random() / (double)RAND_MAX)

void random_fill_LR(int nU, int nI, int nF) {
    srandom(0);
    for(int i = 0; i < nU; i++) for(int j = 0; j < nF; j++)
            L[i][j] = RAND01 / (double) nF; for(int i = 0; i < nF; i++)
        for(int j = 0; j < nI; j++)
            R[i][j] = RAND01 / (double) nF;
}

// A = i Items, u Users, Items and Users
// L = u Users, f Features, Users and Features
// R = f Features, i Items, Features and Items
// B = i Items, u Users, Items and Users
// B = L * R

// Initialize L and R with random_fill_LR
// Make a copy of L and R

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }

    printf("inFile: %s\n", argv[1]);

    std::ifstream inFile(argv[1]);
//    if (inFile.is_open()) {

        int lineNumber = 1;
        for (std::string line; std::getline(inFile, line);) {

            std::istringstream iss(line);

            if (lineNumber < 4) {
                switch (lineNumber) {
                    case 1:
//                        std::cout << "Before stoi" << " " << lineNumber << std::endl;
//                        std::cout << line << std::endl;
                        numberOfIterations = std::stoi(line);
//                        std::cout << "After stoi" << " " << lineNumber << std::endl;
                    case 2:
                        convergenceCoefficient = std::stod(line);
                    case 3:
                        numberOfLatentFeatures = std::stoi(line);
                }
            } else if (lineNumber == 4) {
                std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                 std::istream_iterator<std::string>());
                // TODO: OK
                numberOfUsers = std::stoi(results[0]);
                // TODO: OK
                numberOfItems = std::stoi(results[1]);
                // TODO: OK
                numberOfNonZeroElements = std::stoi(results[2]);

                std::vector<std::vector<double>> LOCAL_A(numberOfUsers, std::vector<double>(numberOfItems));

                for (int i = 0; i < numberOfUsers; i++) {
                    for (int j = 0; j < numberOfItems; j++) {
                        LOCAL_A[i][j] = 0;
                    }
                }

//                std::cout << std::endl;
                for (int i = 0; i < numberOfUsers; i++) {
                    for (int j = 0; j < numberOfItems; j++) {
                        LOCAL_A[i][j] = 0;
//                        std::cout << std::fixed << std::setprecision(6) << LOCAL_A[i][j] << " ";
                    }
                }
//                std::cout << std::endl;

                // TODO: OK
                A = LOCAL_A;

            } else {
                std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                 std::istream_iterator<std::string>());
                int userIndex = std::stoi(results[0]);
                int itemIndex = std::stoi(results[1]);
                double element = std::stod(results[2]);

                // TODO: OK
                A[userIndex][itemIndex] = element;
            }
//            std::cout << line << '\n';
            lineNumber++;
        }
        inFile.close();
//    }

    std::vector<std::vector<double>> LOCAL_R(numberOfLatentFeatures, std::vector<double>(numberOfItems));
    std::vector<std::vector<double>> LOCAL_L(numberOfUsers, std::vector<double>(numberOfLatentFeatures));
    R = LOCAL_R;
    L = LOCAL_L;

    random_fill_LR(numberOfUsers, numberOfItems, numberOfLatentFeatures);

    std::vector<std::vector<double>> LOCAL_B(numberOfUsers, std::vector<double>(numberOfItems));

    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            LOCAL_B[i][j] = 0;
        }
    }

    // multiply L and R to get B
    for (int user = 0; user < numberOfUsers; ++user) {
        for (int item = 0; item < numberOfItems; ++item) {
            for (int latentFeature = 0; latentFeature < numberOfLatentFeatures; ++latentFeature) {
                LOCAL_B[user][item] += L[user][latentFeature] * R[latentFeature][item];
            }
        }
    }

    B = LOCAL_B;


    // print the matrices to a file
    std::ofstream matFile;
    matFile.open("./instances/test.mats");


    matFile << "Initial matrix A" << std::endl;
    std::cout << "Initial matrix A" << std::endl;
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            matFile << std::fixed << std::setprecision(6) << A[i][j] << " ";
            std::cout << std::fixed << std::setprecision(6) << A[i][j] << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Initial matrix L" << std::endl;
    std::cout << "Initial matrix L" << std::endl;
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfLatentFeatures; j++) {
            matFile << std::fixed << std::setprecision(6) << L[i][j] << " ";
            std::cout << std::fixed << std::setprecision(6) << L[i][j] << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Initial matrix R" << std::endl;
    std::cout << "Initial matrix R" << std::endl;
    for (int i = 0; i < numberOfLatentFeatures; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            matFile << std::fixed << std::setprecision(6) << R[i][j] << " ";
            std::cout << std::fixed << std::setprecision(6) << R[i][j] << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile << "Initial matrix B" << std::endl;
    std::cout << "Initial matrix B" << std::endl;
    for (int i = 0; i < numberOfUsers; i++) {
        for (int j = 0; j < numberOfItems; j++) {
            matFile << std::fixed << std::setprecision(6) << B[i][j] << " ";
            std::cout << std::fixed << std::setprecision(6) << B[i][j] << " ";
        }
        matFile << std::endl;
        std::cout << std::endl;
    }

    matFile.close();


    return 0;
}