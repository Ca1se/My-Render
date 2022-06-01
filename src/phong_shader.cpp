#include "phong_shader.hpp"
#include "util.hpp"

Vector4f Shader::vertexShader(const Vector3f& coords) const noexcept {
    return mvp * toVector4f(coords, 1);
}

Vector3f Shader::fragmentShader(float alpha, float beta, float gamma, float corrector) const noexcept {
    Vector3f pos = (world_coords[0] * alpha / homo_coords[0].w() + 
            world_coords[1] * beta / homo_coords[1].w() + 
            world_coords[2] * gamma / homo_coords[2].w()) * corrector;
    Vector3f normal = (normals[0] * alpha / homo_coords[0].w() + 
            normals[1] * beta / homo_coords[1].w() + 
            normals[2] * gamma / homo_coords[2].w()) * corrector;
    Vector2f uv = (uvs[0] * alpha / homo_coords[0].w() + 
            uvs[1] * beta / homo_coords[1].w() + 
            uvs[2] * gamma / homo_coords[2].w()) * corrector;
    
    
    Vector3f kdi = kd;
    if(texture) {
        auto color = texture->sample(uv[0], uv[1]);
        kdi = { color[0] / 255.f, color[1] / 255.f, color[2] / 255.f };
    }
    
    Vector3f light_vec = light_pos - pos;
    Vector3f view_vec = viewer_pos - pos;
    Vector3f light_norm = light_vec.normalized();
    Vector3f view_norm = view_vec.normalized();
    // wont use it
    float square_distance = light_vec.squareNorm();
    normal.normalize();

    Vector3f ambient = ka.cwiseProduct(kdi);
    Vector3f diffuse = kdi.cwiseProduct(light_intensity /* / square_distance */ * std::max(0.f, light_norm.dot(normal)));
    // Vector3f specular = ks.cwiseProduct(light_intensity /* / square_distance */ * std::pow(std::max(0.f, Vector3f{light_norm + view_norm}.normalized().dot(normal)), 100));
    Vector3f specular = Vector3f::Zero();

    return (ambient + diffuse + specular) * 255.f;
}
