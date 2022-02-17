#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include <algorithm>
#include <cstring>
#include <limits>
#include <vector>
#include <array>
#include "matrix.hpp"
#include "model.hpp"
#include "macro.hpp"
#include "phong_shader.hpp"

enum Plane {
    MINIMAL,
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    NEAR,
    FAR
};

struct Payload {
    Vector4f homo_coords[3];
    Vector3f world_coords[3];
    Vector3f normals[3];
    Vector2f uvs[3];

    Vector4f clipped_coords_a[CLIPPED_MAX];
    Vector3f clipped_world_coords_a[CLIPPED_MAX];
    Vector3f clipped_normals_a[CLIPPED_MAX];
    Vector2f clipped_uvs_a[CLIPPED_MAX];

    Vector4f clipped_coords_b[CLIPPED_MAX];
    Vector3f clipped_world_coords_b[CLIPPED_MAX];
    Vector3f clipped_normals_b[CLIPPED_MAX];
    Vector2f clipped_uvs_b[CLIPPED_MAX];
};

class Pipeline {
private:
    size_t width;
    size_t height;
    Payload payload;
    std::vector<float> zbuffer;
    std::vector<std::uint8_t> framebuffer;

public:
    void setRenderingSize(size_t width, size_t height) noexcept {
        this->width = width;
        this->height = height;
        zbuffer.resize(width * height);
        framebuffer.resize(width * height * 4);
    }

    void clearBuffer() noexcept {
        std::fill(zbuffer.begin(), zbuffer.end(), std::numeric_limits<float>::infinity());
        for(std::uint32_t i = 0; i < framebuffer.size(); i += 4) {
            *(std::uint32_t*)(framebuffer.data() + i) = 0xff000000;
        }
    }

    void renderingModel(const Model& model, Shader shader);
    std::uint8_t* data() noexcept { return framebuffer.data(); }

private:
    void prepareVertex(const std::array<int, 3>& tri_index, Shader& shader) noexcept;
    int homogeneous_clip();
    int clipWithPlane(Plane clip_plane, int vertex_num);
    void rasterize(Shader shader);
    void setColor(int x, int y, const Vector3f& color) noexcept;
};


#endif