//
// Created by Pedro Rio on 19/05/2020.
//

#include "testFunc.h"

void testFunc(double *&testArray) {
    delete[] testArray;
    testArray = new double[20];
};
