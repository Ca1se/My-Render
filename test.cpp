#include <iostream>
#include <utility>
#include "matrix.hpp"

int main() {
    Matrix<float, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    for(auto it: m) {
        std::cout << it << std::endl;
    }
    return 0;
}