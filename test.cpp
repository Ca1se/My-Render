#include "matrix.hpp"
#include <iostream>


int main() {
    Matrix<float, 2, 3> A;
    Matrix3f B;
    A << 1, 2, 3,
         4, 5, 6;
    B << 100, 10, 1,
         10, 100, 1,
         1, 10, 100;
    std::cout << (A * B) << std::endl;
    return 0;
}