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

Vector4f toVector4f(const Vector3f& vec3, float w) noexcept;
void setPhongInfo(Shader& shader) noexcept;
void updateShader(Shader& shader, const Camera& camera, const Matrix4f& perspective_matrix) noexcept;
void printMissArg(const char* name) noexcept;
void printWrongArg(const char* name, const char* arg) noexcept;
Vector2f rotatePoint2D(const Vector2f& point, float angle, const Vector2f& origin) noexcept;

#endif