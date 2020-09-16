#include <gtest/gtest.h>
#include "matFact.h"

TEST(matFact, smallInstance) {
    std::string fileName = "inst0";
    std::string instancesPath(std::getenv("TECNICO"));
    instancesPath += "/distributed_and_parallel_computing/instances/";
    std::string outputFileName = instancesPath + fileName + ".out";
    std::string inputFileName = instancesPath + fileName + ".in";

    std::stringstream fileBuffer;
    std::string fileOutput;

    std::stringstream buffer;
    std::string output;

    std::ifstream file(outputFileName);
    fileBuffer << file.rdbuf();
    fileOutput = fileBuffer.str();
    file.close();

    std::streambuf *sbuf = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());
    matFact(inputFileName);
    output = buffer.str();
    std::cout.rdbuf(sbuf);

//    std::cout << fileOutput << std::endl;
//    std::cout << output << std::endl;s
    EXPECT_EQ(output, fileOutput);
}

TEST(matFact, largeInstance) {
    std::string fileName = "inst30-40-10-2-10";
    std::string instancesPath(std::getenv("TECNICO"));
    instancesPath += "/distributed_and_parallel_computing/instances/";
    std::string outputFileName = instancesPath + fileName + ".out";
    std::string inputFileName = instancesPath + fileName + ".in";

    std::stringstream fileBuffer;
    std::string fileOutput;

    std::stringstream buffer;
    std::string output;

    std::ifstream file(outputFileName);
    fileBuffer << file.rdbuf();
    fileOutput = fileBuffer.str();
    file.close();

    std::streambuf *sbuf = std::cout.rdbuf();
    std::cout.rdbuf(buffer.rdbuf());
    matFact(inputFileName);
    output = buffer.str();
    std::cout.rdbuf(sbuf);

//    std::cout << fileOutput << std::endl;
//    std::cout << output << std::endl;
    EXPECT_EQ(output, fileOutput);
}