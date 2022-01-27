#include "math_utils.hpp"
#include <iostream>


int main() {
     Vector3f v1{1, 2, 3};
     Matrix<float, 1, 3> v2{1, 2, 3};
     std::cout << (v1 * v2) << std::endl;
     return 0;
}