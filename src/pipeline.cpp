#include <array>
#include <iostream>
#include <tuple>
#include "pipeline.hpp"
#include "macro.hpp"
#include "model.hpp"
#include "util.hpp"

inline void Pipeline::prepareVertex(const std::array<int, 3>& tri_index, Shader& shader) noexcept {
    for(int i = 0; i < 3; i++) {
        payload.homo_coords[i] = payload.clipped_coords_b[tri_index[i]];

        shader.homo_coords[i]  = payload.homo_coords[i];
        shader.world_coords[i] = payload.clipped_world_coords_b[tri_index[i]];
        shader.normals[i]      = payload.clipped_normals_b[tri_index[i]];
        shader.uvs[i]          = payload.clipped_uvs_b[tri_index[i]];
    }
}

void Pipeline::renderingModel(const Model& model, Shader shader) {
    for(int i = 0; i < model.faces.size(); i++) {
        auto vertex = model.faces[i].vertex;
        for(int j = 0; j < 3; j++) {
            payload.world_coords[j] = model.vertices[vertex[j].vertex_index];
            payload.homo_coords[j]  = shader.vertexShader(payload.world_coords[j]);
            payload.normals[j]      = model.normals[vertex[j].normal_index];
            payload.uvs[j]          = model.uv_coords[vertex[j].uv_index];

            payload.clipped_coords_a[j] = payload.homo_coords[j];
            payload.clipped_world_coords_a[j] = payload.world_coords[j];
            payload.clipped_normals_a[j] = payload.normals[j];
            payload.clipped_uvs_a[j] = payload.uvs[j];
        }

        int vertex_num = homogeneous_clip();

        for(int j = 1; j < vertex_num - 1; j++) {
            prepareVertex({0, j, j + 1}, shader);
            rasterize(shader);
        }
    }
}

static bool is_inside_plane(Plane clip_plane, const Vector4f& vertex) {
    switch (clip_plane) {
        case MINIMAL:
            return vertex.w() <= -MINIMAL_VAL;
        case RIGHT:
            return vertex.x() >= vertex.w();
        case LEFT:
            return vertex.x() <= -vertex.w();
        case TOP:
            return vertex.y() >= vertex.w();
        case BOTTOM:
            return vertex.y() <= -vertex.w();
        case NEAR:
            return vertex.z() >= vertex.w();
        case FAR:
            return vertex.z() <= -vertex.w();
        default:
            return false;
    }
}

// for the deduction of intersection ratio
// refer to: https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
static float get_intersect_ratio(Vector4f prev, Vector4f curv, Plane clip_plane) {
	switch (clip_plane) 
	{
		case MINIMAL:
			return (prev.w() + MINIMAL_VAL) / (prev.w() - curv.w());
		case RIGHT:
			return (prev.w() - prev.x()) / ((prev.w() - prev.x()) - (curv.w() - curv.x()));
		case LEFT:
			return (prev.w() + prev.x()) / ((prev.w() + prev.x()) - (curv.w() + curv.x()));
		case TOP:
			return (prev.w() - prev.y()) / ((prev.w() - prev.y()) - (curv.w() - curv.y()));
		case BOTTOM:
			return (prev.w() + prev.y()) / ((prev.w() + prev.y()) - (curv.w() + curv.y()));
		case NEAR:
			return (prev.w() - prev.z()) / ((prev.w() - prev.z()) - (curv.w() - curv.z()));
		case FAR:
			return (prev.w() + prev.z()) / ((prev.w() + prev.z()) - (curv.w() + curv.z()));
		default:
			return 0;
	}
}

int Pipeline::clipWithPlane(Plane clip_plane, int vertex_num) {
    bool is_odd = (bool) (clip_plane % 2);

    auto& in_coords         = (is_odd ? payload.clipped_coords_b : payload.clipped_coords_a);
    auto& in_world_coords   = (is_odd ? payload.clipped_world_coords_b : payload.clipped_world_coords_a);
    auto& in_normals        = (is_odd ? payload.clipped_normals_b : payload.clipped_normals_a);
    auto& in_uvs            = (is_odd ? payload.clipped_uvs_b : payload.clipped_uvs_a);

    auto& out_coords        = (is_odd ? payload.clipped_coords_a : payload.clipped_coords_b);
    auto& out_world_coords  = (is_odd ? payload.clipped_world_coords_a : payload.clipped_world_coords_b);
    auto& out_normals       = (is_odd ? payload.clipped_normals_a : payload.clipped_normals_b);
    auto& out_uvs           = (is_odd ? payload.clipped_uvs_a : payload.clipped_uvs_b);

    int num = 0;

    for(int i = 0; i < vertex_num; i++) {
        int v1 = i;
        int v2 = (i + 1) % vertex_num;
        bool v1_inside = is_inside_plane(clip_plane, in_coords[v1]);
        bool v2_inside = is_inside_plane(clip_plane, in_coords[v2]);

        if(v1_inside != v2_inside) {
            float ratio             = get_intersect_ratio(in_coords[v1], in_coords[v2], clip_plane);
            out_coords[num]         = vectorInterpolate(in_coords[v1], in_coords[v2], ratio);
            out_world_coords[num]   = vectorInterpolate(in_world_coords[v1], in_world_coords[v2], ratio);
            out_normals[num]        = vectorInterpolate(in_normals[v1], in_normals[v2], ratio);
            out_uvs[num++]          = vectorInterpolate(in_uvs[v1], in_uvs[v2], ratio);
        }

        if(v2_inside) {
            out_coords[num]         = in_coords[v2];
            out_world_coords[num]   = in_world_coords[v2];
            out_normals[num]        = in_normals[v2];
            out_uvs[num++]          = in_uvs[v2];
        }
    }

    return num;
}

int Pipeline::homogeneous_clip() {
    int num = 3;
    num = clipWithPlane(MINIMAL, num);
    num = clipWithPlane(RIGHT, num);
    num = clipWithPlane(LEFT, num);
    num = clipWithPlane(TOP, num);
    num = clipWithPlane(BOTTOM, num);
    num = clipWithPlane(NEAR, num);
    return clipWithPlane(FAR, num);
}

inline std::tuple<float, float, float> computeBarycentricCoords2D(float x, float y, const Vector3f (&v)[3]) noexcept {
    float alpha = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / 
            (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float beta = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / 
            (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    return std::make_tuple(alpha, beta, 1 - alpha - beta);
}

inline bool inside_triangle(float alpha, float beta, float gamma) noexcept {
    return (alpha >= 0) && (beta >= 0) && (gamma >= 0);
}

void Pipeline::rasterize(Shader shader) {
    Vector3f screen_pos[3];

    for(int i = 0; i < 3; i++) {
        screen_pos[i].x() = 0.5 * (width - 1) * (payload.homo_coords[i].x() / payload.homo_coords[i].w() + 1);
        screen_pos[i].y() = 0.5 * (height - 1) * (payload.homo_coords[i].y() / payload.homo_coords[i].w() + 1);
        screen_pos[i].z() = -payload.homo_coords[i].w();
    }

    int x_max = 0;
    int x_min = width;
    int y_max = 0;
    int y_min = height;

    float x, y;
    for(auto & screen_po : screen_pos) {
        x = screen_po.x();
        y = screen_po.y();
        x_max = std::max(x_max, (int) std::ceil(x));
        x_min = std::min(x_min, (int) x);
        y_max = std::max(y_max, (int) std::ceil(y));
        y_min = std::min(y_min, (int) y);
    }

    for(int y = y_min; y <= y_max; y++) {
        for(int x = x_min; x <= x_max; x++) {
            auto [alpha, beta, gamma] = computeBarycentricCoords2D((float) (x + 0.5), (float) (y + 0.5), screen_pos);
            if(inside_triangle(alpha, beta, gamma)) {
                int index = y * width + x;
                float corrector = 1 / (alpha / payload.homo_coords[0].w() + beta / payload.homo_coords[1].w() + gamma / payload.homo_coords[2].w());
                float z = -corrector;
                if(zbuffer[index] > z) {
                    zbuffer[index] = z;
                    Vector3f color = shader.fragmentShader(alpha, beta, gamma, corrector);
                    for(int i = 0; i < 3; i++) {
                        color[i] = std::max(0.f, std::min(255.f, color[i]));
                    }
                    setColor(x, y, color);
                }
            }
        }
    }
}

void Pipeline::setColor(int x, int y, const Vector3f& color) noexcept {
    int index = ((height - y - 1) * width + x) * 4;

    framebuffer[index]     = color[0];
    framebuffer[index + 1] = color[1];
    framebuffer[index + 2] = color[2];
}