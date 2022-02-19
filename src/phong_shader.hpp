#ifndef _PHONG_SHADER_HPP_
#define _PHONG_SHADER_HPP_

#include <memory>
#include "material.hpp"
#include "matrix.hpp"


struct Shader {
    Matrix4f mvp;

    Vector4f homo_coords[3];
    Vector3f world_coords[3];
    Vector3f normals[3];
    Vector2f uvs[3];

    Vector3f ka;
    Vector3f kd;
    Vector3f ks;

    Vector3f viewer_pos;
    Vector3f light_pos;
    Vector3f light_intensity;

    std::shared_ptr<Texture> texture;

    Vector4f vertexShader(const Vector3f& coords) const noexcept;
    Vector3f fragmentShader(float alpha, float beta, float gamma, float corrector) const noexcept;
};


#endif