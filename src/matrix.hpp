// Author: Wu Zihao                 
// Github: https://github.com/Ca1se 
//
// This is a matrix library containing both matrix and vector parts.
// This library is used for calculations for my computer graphics practice project.
// This library require C++17

#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>


inline constexpr bool equalValue(size_t t1, size_t t2) {
    return (t1 == t2);
}

// Class Matrix (and Vector) only accept types
// that can be used for arithmetic.
template <typename T, size_t Rows, size_t Cols,
        bool = equalValue(Rows, Cols),
        bool = std::is_arithmetic<T>::value>
class Matrix;

template <typename U>
using IsNumber = typename std::enable_if<std::is_arithmetic<U>::value, int>::type;

// The Base class, This class contains functions
// that are available when the number of rows and
// columns of the matrix are equal and when they are not.
template <typename T, size_t Rows, size_t Cols>
class Matrix<T, Rows, Cols, false, true> {
private:
    static constexpr size_t size_ = (Rows * Cols);

    T data_[size_];

public:
    Matrix() = default;
    ~Matrix() = default;

    Matrix(const Matrix<T, Rows, Cols>& other) {
        std::copy(other.begin(), other.end(), begin());
    }

    Matrix(const std::initializer_list<T>& elements) {
        assert(elements.size() == size());
        std::copy(elements.begin(), elements.end(), begin());
    }

    static Matrix<T, Rows, Cols> Zero() {
        Matrix<T, Rows, Cols> ret;
        for(int i = 0; i < ret.size(); i++) {
            ret[i] = 0;
        }
        return ret;
    }

public:
    constexpr size_t size() const noexcept { return size_; }

    constexpr T* data() const noexcept { return data_; }

    constexpr size_t rows() const noexcept { return Rows; }

    constexpr size_t cols() const noexcept { return Cols; }

    T* begin() noexcept { return data_; }

    const T* begin() const noexcept { return data_; }

    T* end() noexcept { return (data_ + size_); }

    const T* end() const noexcept { return (data_ + size_); }

    T& at(int row_index, int col_index) noexcept {
        assert((row_index < Rows) && (col_index < Cols));
        return data_[row_index * Cols + col_index];
    }

    const T& at(int row_index, int col_index) const noexcept {
        assert((row_index < Rows) && (col_index < Cols));
        return data_[row_index * Cols + col_index];
    }

    T& at(int index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    const T& at(int index) const noexcept {
        assert(index < size_);
        return data_[index];
    }

    T& operator() (size_t row_index, size_t col_index) noexcept {
        return data_[row_index * Cols + col_index];
    }

    const T& operator() (size_t row_index, size_t col_index) const noexcept {
        return data_[row_index * Cols + col_index];
    }

    T& operator[] (size_t index) noexcept {
        return data_[index];
    }

    const T& operator[] (size_t index) const noexcept {
        return data_[index];
    }

    template<typename U, size_t OtherCols, IsNumber<U> = 0>
    Matrix<T, Rows, OtherCols> operator*
            (const Matrix<U, Cols, OtherCols>& other_matrix)
            const noexcept {
        Matrix<T, Rows, OtherCols> ret;

        for(int i = 0; i < Rows; i++) {
            for(int j = 0; j < OtherCols; j++) {
                T t{};
                for(int k = 0; k < Cols; k++) {
                    t += (*this)(i, k) * other_matrix(k, j);
                }
                ret(i, j) = t;
            }
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator* (U number) const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] *= number;
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator*= (U number) noexcept {
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] *= number;
        }

        return *this;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator/ (U number) const noexcept {
        assert(number != 0);
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] /= number;
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator/= (U number) noexcept {
        assert(number != 0);
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] /= number;
        }

        return *this;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator+
            (const Matrix<U, Rows, Cols>& other_matrix)
            const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] += other_matrix[i];
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator+ (U number) const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] += number;
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator+=
            (const Matrix<U, Rows, Cols>& other_matrix)
            noexcept {
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] += other_matrix[i];
        }

        return *this;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator+= (U number) noexcept {
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] += number;
        }

        return *this;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator-
            (const Matrix<U, Rows, Cols>& other_matrix)
            const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] -= other_matrix[i];
        }

        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> operator- (U number) const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] -= number;
        }
        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator-=
            (const Matrix<U, Rows, Cols>& other_matrix)
            noexcept {
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] -= other_matrix[i];
        }

        return *this;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols>& operator-= (U number) noexcept {
        for(int i = 0; i < this->size(); i++) {
            (*this)[i] -= number;
        }
        return *this;
    }

    Matrix<T, Cols, Rows> transpose() const noexcept {
        Matrix<T, Cols, Rows> ret;
        for(int i = 0; i < rows(); i++) {
            for(int j = 0; j < cols(); j++) {
                ret(j, i) = (*this)(i, j);
            }
        }
        return ret;
    }

    template <typename U, IsNumber<U> = 0>
    Matrix<T, Rows, Cols> cwiseProduct
            (const Matrix<U, Rows, Cols>& other_matrix)
            const noexcept {
        Matrix<T, Rows, Cols> ret(*this);

        for(int i = 0; i < ret.size(); i++) {
            ret[i] *= other_matrix[i];
        }

        return ret;
    }

};

// This class is used for comma initialization of matrix
template <typename T, size_t Rows, size_t Cols>
class CommaInitializer {
private:
    Matrix<T, Rows, Cols>& target_;
    Matrix<T, Rows, Cols> tmp_;
    int init_count_;
public:
    template<typename U>
    CommaInitializer(Matrix<T, Rows, Cols>& t, U val):
            target_(t), init_count_(1) {
        tmp_.at(0) = val;
    }

    ~CommaInitializer() {
        assert(init_count_ == target_.size());
        std::swap(target_, tmp_);
    }

    template<typename U>
    CommaInitializer& operator, (U e) {
        tmp_.at(init_count_) = e;
        init_count_++;
        return *this;
    }
};

template <typename T, size_t Rows, size_t Cols>
inline std::ostream& operator<< (std::ostream& os, const Matrix<T, Rows, Cols>& matrix) {
    for(int i = 0; i < Rows; i++) {
        if(i != 0) os << "\n";
        for(int j = 0; j < Cols; j++) {
            os << matrix(i, j) << " ";
        }
    }
    return os;
}

template <typename T, size_t Rows, size_t Cols, typename U>
inline CommaInitializer<T, Rows, Cols> operator<< (Matrix<T, Rows, Cols>& matrix, U val) {
    return CommaInitializer<T, Rows, Cols>(matrix, val);
}

// This class contains functions that are only available
// when the number of rows and the number of cols are equal.
template <typename T, size_t Rows, size_t Cols>
class Matrix<T, Rows, Cols, true, true>: public Matrix<T, Rows, Cols, false, true> {
private:
    typedef Matrix<T, Rows, Cols, false, true> Base;

public:
    Matrix() = default;

    Matrix(const std::initializer_list<T>& elements): Base::Matrix(elements) {}

    static Matrix<T, Rows, Cols> Identity() {
        Matrix<T, Rows, Cols> ret;

        for(int i = 0; i < Rows; i++) {
            for(int j = 0; j < Cols; j++) {
                ret(i, j) = 0;
                if(i == j)  ret(i, j) = 1;
            }
        }
        return ret;
    }

};

template <typename T, typename U, size_t Rows, size_t Cols, IsNumber<T> = 0>
inline Matrix<U, Rows, Cols> operator*(T number, const Matrix<U, Rows, Cols>& matrix) {
    return matrix * number;
}

template <typename T, typename U, size_t Rows, size_t Cols, IsNumber<T> = 0>
inline Matrix<U, Rows, Cols> operator+(T number, const Matrix<U, Rows, Cols>& matrix) {
    return matrix + number;
}


// This class contains functions that are used for
// vector operations.
// Some functions in this class are unavailable in
// some cases, such as function cross is unavailable
// when _Size != 3, and if you use it, you will get
// a compile error.
template <typename T, size_t Size>
class Vector: public Matrix<T, Size, 1> {
private:
    typedef Matrix<T, Size, 1> Base;

public:
    Vector() = default;

    Vector(const Matrix<T, Size, 1>& matrix): Base::Matrix(matrix) {}

    Vector(const std::initializer_list<T>& elements): Base::Matrix(elements) {}

public:
    T squareNorm() const noexcept {
        T ret{};
        for(auto it: *this) {
            ret += it * it;
        }
        return ret;
    }

    T norm() const noexcept {
        return std::sqrt(squareNorm());
    }

    /*
    void normalize() noexcept {
        auto n = norm();
        assert(n != 0);
        for(auto& it: *this) {
            it /= n;
        }
    }
    */

    Vector<T, Size> normalized() const noexcept {
        Vector<T, Size> ret(*this);
        auto n = norm();
        assert(n != 0);

        for(auto& it: ret) {
            it /= n;
        }

        return ret;
    }

    void normalize() noexcept {
        (*this) = this->normalized();
    }

    template <typename U>
    T dot(const Vector<U, Size>& other_vector) const noexcept {
        T ret{};
        for(int i = 0; i < Base::size(); i++) {
            ret += (*this)[i] * other_vector[i];
        }
        return ret;
    }

    // Only available when vector size == 3
    template <typename U>
    Vector<T, 3> cross(const Vector<U, 3>& other_vector) const noexcept {
        static_assert(Size == 3);

        return Vector<T, 3>{
            (*this)[1] * other_vector[2] - (*this)[2] * other_vector[1],
            (*this)[2] * other_vector[0] - (*this)[0] * other_vector[2],
            (*this)[0] * other_vector[1] - (*this)[1] * other_vector[0]
        };
    }

    T& x() noexcept {
        static_assert(Size > 0);
        return (*this)[0];
    }

    const T& x() const noexcept {
        static_assert(Size > 0);
        return (*this)[0];
    }

    // Only available when vector size > 1
    T& y() noexcept {
        static_assert(Size > 1);
        return (*this)[1];
    }

    // Only available when vector size > 1
    const T& y() const noexcept {
        static_assert(Size > 1);
        return (*this)[1];
    }

    // Only available when vector size > 2
    T& z() noexcept {
        static_assert(Size > 2);
        return (*this)[2];
    }

    // Only available when vector size > 2
    const T& z() const noexcept {
        static_assert(Size > 2);
        return (*this)[2];
    }

    // Only available when vector size > 3
    T& w() noexcept {
        static_assert(Size > 3);
        return (*this)[3];
    }

    // Only available when vector size > 3
    const T& w() const noexcept {
        static_assert(Size > 3);
        return (*this)[3];
    }

};

typedef Matrix<int, 2, 2> Matrix2i;
typedef Matrix<int, 3, 3> Matrix3i;
typedef Matrix<int, 4, 4> Matrix4i;

typedef Matrix<float, 2, 2> Matrix2f;
typedef Matrix<float, 3, 3> Matrix3f;
typedef Matrix<float, 4, 4> Matrix4f;

typedef Matrix<double, 2, 2> Matrix2d;
typedef Matrix<double, 3, 3> Matrix3d;
typedef Matrix<double, 4, 4> Matrix4d;

typedef Vector<int, 2> Vector2i;
typedef Vector<int, 3> Vector3i;
typedef Vector<int, 4> Vector4i;

typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 4> Vector4f;

typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;
typedef Vector<double, 4> Vector4d;

#endif // _matrix_HPP_