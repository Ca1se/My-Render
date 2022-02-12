#include <cmath>
#include "util.hpp"
#include "matrix.hpp"
#include "macro.hpp"

inline Matrix4f calViewMatrix(const Camera& camera) {
    Vector3f w = camera.gaze / -camera.gaze.norm();
    Vector3f txw = camera.view_up.cross(w);
    Vector3f u = txw / txw.norm();
    Vector3f v = w.cross(u);
    const Vector3f& e = camera.position;

    return Matrix4f{
        u.x(), u.y(), u.z(), -u.x() * e.x() - u.y() * e.y() - u.z() * e.z(),
        v.x(), v.y(), v.z(), -v.x() * e.x() - v.y() * e.y() - v.z() * e.z(),
        w.x(), w.y(), w.z(), -w.x() * e.x() - w.y() * e.y() - w.z() * e.z()
    };
}

inline Matrix4f calPerspectiveMatrix(float fov, float aspect, float near, float far) {
    float r = fov / 2 / 180 * PI;
    float top = -tan(r) * near;
    float right = top * aspect;

    return Matrix4f{
        near / right, 0, 0, 0,
        0, near / top, 0, 0,
        0, 0, (near + far) / (near - far), -2 * near * far / (near - far),
        0, 0, 1, 0
    };
}