#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include <vector>
#include "matrix.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "macro.hpp"

enum Plane {
    MINIMAL,
    TOP,
    BOTTOM,
    RIGHT,
    LEFT,
    NEAR,
    FAR
};

struct Payload {
    Vector4f coords[3];
    Vector3f world_coords[3];
    Vector3f normals[3];
    Vector2f uvs[3];

    Vector4f clipped_coords[CLIPPED_MAX];
    Vector3f clipped_world_coords[CLIPPED_MAX];
    Vector3f clipped_normals[CLIPPED_MAX];
    Vector2f clipped_uvs[CLIPPED_MAX];
};

class Pipeline {
private:
    static Payload payload;

public:
    static void draw(const Model &model, VertexShader shader);

private:
    static int homogeneous_clip();
    static int clipWithPlane(Plane clip_plane, int vertex_num);
};


#endif