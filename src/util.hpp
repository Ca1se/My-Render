#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <cstddef>
#include "camera.hpp"
#include "matrix.hpp"
#include "phong_shader.hpp"

Matrix4f calViewMatrix(const Camera& camera);
Matrix4f calPerspectiveMatrix(float fov, float aspect, float near, float far);

template <typename T, size_t Size>
inline Vector<T, Size> vectorInterpolate(const Vector<T, Size>& start, 
        const Vector<T, Size>& end, float t) noexcept {
    return start + (end - start) * t;
}

int getIndex(int x, int y, int width, int height) noexcept;
Vector4f toVector4f(const Vector3f& vec3, float w) noexcept;
void setPhongInfo(Shader& shader, const Camera& camera) noexcept;

#endif