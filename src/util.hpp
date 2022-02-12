#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include "camera.hpp"
#include "matrix.hpp"

Matrix4f calViewMatrix(const Camera& camera);
Matrix4f calPerspectiveMatrix(float fov, float aspect, float near, float far);

#endif