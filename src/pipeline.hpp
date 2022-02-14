#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include <vector>
#include "matrix.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "macro.hpp"

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
    Vector4f coords[3];
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
    static size_t width;
    static size_t height;
    static Payload payload;
    static VertexShader vertex_shader;
    static FragmentShader fragment_shader;
    static std::vector<float> zbuffer;
    static std::vector<std::uint8_t> framebuffer;

public:
    static void set_vertex_shader(const VertexShader& shader) noexcept { vertex_shader = shader; }
    static void set_fragment_shader(const FragmentShader& shader) noexcept { fragment_shader = shader; }
    static void setRenderingSize(size_t width, size_t height) noexcept {
        Pipeline::width = width;
        Pipeline::height = height;
        zbuffer.resize(width * height);
        framebuffer.resize(width * height * 4);
    }

    static void draw(const Model &model);

private:
    static void prepareVertex(int index1, int index2, int index3) noexcept;
    static int homogeneous_clip();
    static int clipWithPlane(Plane clip_plane, int vertex_num);
    static void rasterize();
};


#endif