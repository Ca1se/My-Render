#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include <functional>
#include "matrix.hpp"

struct VertexShader {
    Matrix4f mvp_matrix;
    Vector4f process(const Vector3f& coord) {
        return mvp_matrix * Vector4f{ coord.x(), coord.y(), coord.z(), 1 };
    }
};

struct FragmentShader {
    Vector3f ka;
    Vector3f kd;
    Vector3f ks;
};

#endif