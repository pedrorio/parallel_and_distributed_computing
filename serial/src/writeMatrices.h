//
// Created by Pedro Rio on 14/03/2020.
//

#ifndef SERIAL_WRITEMATRICES_H
#define SERIAL_WRITEMATRICES_H

#include <iostream>
#include <fstream>
#include <vector>

void writeMatrices(std::string &matrixFileName,
                   std::vector<std::vector<double>> &L, std::vector<std::vector<double>> &R,
                   std::vector<std::vector<double>> &B, int &iterationNumber);


#endif //SERIAL_WRITEMATRICES_H


