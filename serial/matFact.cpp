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

#define RAND01 ((double)random() / (double)RAND_MAX)

void random_fill_LR(int nU, int nI, int nF) {
    srandom(0);
    for (int i = 0; i < nU; i++)
        for (int j = 0; j < nF; j++) {
            std::cout << "user i :" << i << std::endl;
            std::cout << "item j :" << j << std::endl;
            std::cout << "element:" << RAND01 / (double) nF << std::endl;
            L[i][j] = RAND01 / (double) nF;
        }
    std::cout << "L is done" << std::endl;

    for (int i = 0; i < nF; i++)
        for (int j = 0; j < nI; j++)
            R[i][j] = RAND01 / (double) nF;
    std::cout << "R is done" << std::endl;
    
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
    if (inFile.is_open()) {

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
                numberOfItems = std::stoi(results[0]);
                numberOfUsers = std::stoi(results[1]);
                numberOfNonZeroElements = std::stoi(results[2]);

                std::vector<std::vector<double>> LOCAL_A(numberOfUsers, std::vector<double>(numberOfItems));

                for (int i = 0; i < numberOfUsers; i++) {
                    for (int j = 0; j < numberOfItems; j++) {
                        LOCAL_A[i][j] = 0;
                    }
                }

                std::cout << "A was initialised" << std::endl;
                A = LOCAL_A;
            } else {
                std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                                 std::istream_iterator<std::string>());
                int userIndex = std::stoi(results[0]);
                int itemIndex = std::stoi(results[1]);
                double element = std::stod(results[2]);
                A[userIndex][itemIndex] = element;
            }
            std::cout << line << '\n';
            lineNumber++;
        }
        inFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    std::vector<std::vector<double>> LOCAL_R(numberOfLatentFeatures, std::vector<double>(numberOfItems));
    std::vector<std::vector<double>> LOCAL_L(numberOfUsers, std::vector<double>(numberOfLatentFeatures));
    R = LOCAL_R;
    L = LOCAL_L;

    random_fill_LR(numberOfUsers, numberOfItems, numberOfLatentFeatures);
    return 0;
}