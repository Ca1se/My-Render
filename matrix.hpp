#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

template <typename T, size_t row, size_t col>
class Matrix {
public:
    Matrix() = default;
    ~Matrix() = default;

    Matrix(std::initializer_list<T> elements) {
        assert((row * col) == elements.size());
        std::copy(elements.begin(), elements.end(), data_);
    }

    Matrix(const Matrix<T, row, col>& other) {
        std::copy(other.begin(), other.end(), data_);
    }

    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

public:
    constexpr size_t size() const noexcept { return row * col; }

    constexpr T* data() const noexcept { return data_; }

    constexpr size_t rows() const noexcept { return row; }

    constexpr size_t cols() const noexcept { return col;}

    iterator begin() noexcept { return data_; }

    const_iterator begin() const noexcept { return data_; }

    iterator end() noexcept { return (data_ + size()); }

    const_iterator end() const noexcept { return (data_ + size()); }

private:
    T data_[row * col] = {0};
};

#endif