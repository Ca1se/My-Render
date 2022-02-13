#include "pipeline.hpp"
#include "model.hpp"
#include "shader.hpp"

void Pipeline::draw(const Model& model, VertexShader shader) {
    for(int i = 0; i < model.faces.size(); i++) {
        auto vertex = model.faces[i].vertex;
        for(int j = 0; j < 3; j++) {
            payload.world_coords[j] = model.vertices[vertex[j].vertex_index];
            payload.coords[j] = shader.process(payload.world_coords[j]);
            payload.normals[j] = model.normals[vertex[j].normal_index];
            payload.uvs[j] = model.uv_coords[vertex[j].uv_index];
        }

        if(int num = homogeneous_clip(); num >= 3) {

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

static int clipWithPlane(Plane clip_plane, int vertex_num) {
    // unfinished
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