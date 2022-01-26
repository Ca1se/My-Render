#include <iostream>
#include <utility>
#include "matrix.hpp"

int main() {
    Matrix<float, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix<float, 3, 3> m1 = std::move(m);
    for(auto it: m1) {
        std::cout << it << std::endl;
    }
    return 0;
}