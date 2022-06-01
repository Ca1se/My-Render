#include <cmath>
#include <cstdio>
#include "camera.hpp"
#include "util.hpp"
#include "matrix.hpp"
#include "macro.hpp"

Matrix4f calViewMatrix(const Camera& camera) {
    Vector3f w = Vector3f{camera.view - camera.target}.normalized();
    Vector3f u = camera.up.cross(w).normalized();
    Vector3f v = w.cross(u).normalized();
    const Vector3f& e = camera.view;

    return Matrix4f{
        u.x(), u.y(), u.z(), -u.x() * e.x() - u.y() * e.y() - u.z() * e.z(),
        v.x(), v.y(), v.z(), -v.x() * e.x() - v.y() * e.y() - v.z() * e.z(),
        w.x(), w.y(), w.z(), -w.x() * e.x() - w.y() * e.y() - w.z() * e.z(),
        0, 0, 0, 1
    };
}

Matrix4f calProjectionMatrix(float fov, float aspect, float near, float far) {
    float r = fov / 2 / 180 * PI;
    float top = -tan(r) * near;
    float right = top * aspect;

    // perspective
    return Matrix4f{
        near / right, 0, 0, 0,
        0, near / top, 0, 0,
        0, 0, (near + far) / (near - far), -2 * near * far / (near - far),
        0, 0, 1, 0
    };

    /*
    // orthographic
    return Matrix4f {
        -1 / right, 0, 0, 0,
        0, -1 / top, 0, 0,
        0, 0, 2 / (near - far), (near + far) / (far - near),
        0, 0, 1, 0
    };
    */
}

Vector4f toVector4f(const Vector3f& vec3, float w) noexcept {
    return Vector4f{ vec3.x(), vec3.y(), vec3.z(), w };
}

void setPhongInfo(Shader& shader) noexcept {
    shader.ka = { 0.35, 0.35, 0.35 };
    shader.kd = { 0.25, 0.25, 0.25 };
    shader.ks = { 0.7, 0.7, 0.7 };

    shader.light_pos = { 0, 30, -30 };
    shader.light_intensity = { 1, 1, 1 };
}

void updateShader(Shader& shader, const Camera& camera, const Matrix4f& perspective_matrix) noexcept {
    shader.viewer_pos = camera.view;
    shader.mvp = perspective_matrix * calViewMatrix(camera);
}

void printMissArg(const char* name) noexcept {
    printf("%s: missing argument\n", name);
    printf("Try '%s --help' for more information\n", name);
}

void printWrongArg(const char* name, const char* arg) noexcept {
    printf("%s: wrong argument: %s\n", name, arg);
    printf("Try '%s --help' for more information\n", name);
}

Vector2f rotatePoint2D(const Vector2f& point, float angle, const Vector2f& origin) noexcept {
    Vector2f p = point - origin;
    float r = angle / 180 * PI;
    
    float cos_r = std::cos(r);
    float sin_r = std::sin(r);
    Matrix2f rotater = {
        cos_r, -sin_r,
        sin_r,  cos_r
    };
    p = rotater * p;
    return p + origin;
}
