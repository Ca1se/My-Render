#ifndef _MATH_UTILS_HPP_
#define _MATH_UTILS_HPP_

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename _T, size_t _Rows, size_t _Cols>
class CommaInitializer;

static constexpr bool eq(size_t t1, size_t t2) {
    return (t1 == t2);
}

template <typename _T, typename _U>
void unpacker(_T& container, _U&& t, int& pos) {
    container.data_[pos++] = t;
}

template <typename _T, size_t _Rows, size_t _Cols,
        bool = eq(_Rows, _Cols),
        bool = std::is_arithmetic<_T>::value>
class Matrix;

template <typename _T, size_t _Rows, size_t _Cols>
class Matrix<_T, _Rows, _Cols, false, true> {
public:
    typedef _T value_type;
    typedef _T* iterator;
    typedef const _T* const_iterator;
    typedef _T& reference;
    typedef const _T& const_reference;

private:
    static constexpr size_t size_ = (_Rows * _Cols);

    _T data_[size_];

public:
    Matrix() = default;
    ~Matrix() = default;

    template<typename ...Args>
    Matrix(Args&& ...args) {
        static_assert(sizeof...(Args) == (_Rows * _Cols));
        int cnt = 0;
        (unpacker(*this, args, cnt), ...);
    }

    Matrix(const Matrix<_T, _Rows, _Cols>& other) {
        std::copy(other.begin(), other.end(), data_);
    }

    static Matrix<_T, _Rows, _Cols> Zero() {
        Matrix<_T, _Rows, _Cols> ret;
        for(int i = 0; i < ret.size(); i++) {
            ret.data_[i] = 0;
        }
        return ret;
    }

public:
    constexpr size_t size() const noexcept { return size_; }

    constexpr _T* data() const noexcept { return data_; }

    constexpr size_t rows() const noexcept { return _Rows; }

    constexpr size_t cols() const noexcept { return _Cols;}

    iterator begin() noexcept { return data_; }

    const_iterator begin() const noexcept { return data_; }

    iterator end() noexcept { return (data_ + size_); }

    const_iterator end() const noexcept { return (data_ + size_); }

    reference at(int row_index, int col_index) noexcept {
        assert((row_index < _Rows) && (col_index < _Cols));
        return data_[row_index * _Cols + col_index];
    }

    const_reference at(int row_index, int col_index) const noexcept {
        assert((row_index < _Rows) && (col_index < _Cols));
        return data_[row_index * _Cols + col_index];
    }

    template<size_t _OtherCols>
    Matrix<_T, _Rows, _OtherCols> operator* (const Matrix<_T, _Cols, _OtherCols>& other_matrix) const {
        Matrix<_T, _Rows, _OtherCols> ret;

        for(int i = 0; i < _Rows; i++) {
            for(int j = 0; j < _OtherCols; j++) {
                _T t{};
                for(int k = 0; k < _Cols; k++) {
                    t += this->at(i, k) * other_matrix.at(k, j);
                }
                ret.at(i, j) = t;
            }
        }

        return ret;
    }

    template <typename _TR, size_t _RowsR, size_t _ColsR>
    friend class CommaInitializer;

    template <typename _TR, size_t _RowsR, size_t _ColsR>
    friend std::ostream& operator<< (std::ostream& os, const Matrix<_TR, _RowsR, _ColsR>& matrix);

    template <typename _TR, size_t _RowsR, size_t _ColsR, typename _U>
    friend CommaInitializer<_TR, _RowsR, _ColsR> operator<< (Matrix<_TR, _RowsR, _ColsR>& matrix, _U val);

    template <typename _TR, typename _UR>
    friend void unpacker(_TR& container, _UR&& t, int& pos);
};

template <typename _T, size_t _Rows, size_t _Cols>
inline std::ostream& operator<< (std::ostream& os, const Matrix<_T, _Rows, _Cols>& matrix) {
    for(int i = 0; i < _Rows; i++) {
        if(i != 0) os << "\n";
        for(int j = 0; j < _Cols; j++) {
            os << matrix.at(i, j) << " ";
        }
    }
    return os;
}

template <typename _T, size_t _Rows, size_t _Cols, typename _U>
inline CommaInitializer<_T, _Rows, _Cols> operator<< (Matrix<_T, _Rows, _Cols>& matrix, _U val) {
    return CommaInitializer<_T, _Rows, _Cols>(matrix, val);
}



template <typename _T, size_t _Rows, size_t _Cols>
class Matrix<_T, _Rows, _Cols, true, true>: public Matrix<_T, _Rows, _Cols, false, true> {
public:
    template<typename ...Args>
    Matrix(Args&& ...args) {
        static_assert(sizeof...(Args) == (_Rows * _Cols));
        int cnt = 0;
        (unpacker(*this, args, cnt), ...);
    }

    static Matrix<_T, _Rows, _Cols> Identity() {
        Matrix<_T, _Rows, _Cols> ret;

        for(int i = 0; i < _Rows; i++) {
            for(int j = 0; j < _Cols; j++) {
                ret.at(i, j) = 0;
                if(i == j)  ret.at(i, j) = 1;
            }
        }
        return ret;
    }
};

template <typename _T, size_t _Rows, size_t _Cols>
class CommaInitializer {
private:
    Matrix<_T, _Rows, _Cols>& target_;
    Matrix<_T, _Rows, _Cols> tmp_;
    int init_count_;
public:
    template<typename _U>
    CommaInitializer(Matrix<_T, _Rows, _Cols>& t, _U val): 
            target_(t), init_count_(1) {
        tmp_.data_[0] = val;
    }

    ~CommaInitializer() {
        assert(init_count_ == target_.size());
        std::swap(target_, tmp_);
    }

    template<typename _U>
    CommaInitializer& operator, (_U e) {
        tmp_.data_[init_count_] = e;
        init_count_++;
        return *this;
    }
};

template <typename _T, size_t _Size>
class Vector: public Matrix<_T, _Size, 1> {
public:
    template <typename ...Args>
    Vector(Args&& ...args): Matrix<_T, _Size, 1>::Matrix(std::forward<Args>(args)...) {}
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

#endif // _MATH_UTILS_HPP_