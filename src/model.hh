#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <string_view>
#include <vector>
#include <string>
#include "math_utils.hh"

struct VertexInfo {
    int vertex_index;
    int normal_index;
    int uv_index;
};

class Model {
private:
    std::vector<Vector3f> vertices_;
    std::vector<Vector3f> normals_;
    std::vector<Vector2f> uv_coords_;

    // vectices' index
    std::vector<std::array<VertexInfo, 3>> faces_;

public:
    const std::vector<Vector3f>& vertices() const noexcept { return vertices_; }
    const std::vector<Vector3f>& normals() const noexcept { return normals_; }
    const std::vector<Vector2f>& uv_coords() const noexcept { return uv_coords_; }
    const auto& faces() const noexcept { return faces_; }

public:
    friend class ObjLoader;
};


class ObjLoader {
private:
    std::vector<Model> loaded_models_;

    std::string obj_str_;
    size_t now;
    size_t pre;

public:
    ObjLoader& load(const std::string& obj_file_name);
    const std::vector<Model>& loaded_models() const { return loaded_models_; }

private:
    void praseObj();

    std::string_view nextLine() noexcept;

    bool hasNextLine() const noexcept { return now != obj_str_.size(); }

    static void praseVertex(std::string_view vector_line, Vector3f& result) {
        sscanf(vector_line.data(), "v %f %f %f", &result.x(), &result.y(), &result.z());
    }

    static void praseUVCoord(std::string_view coord_line, Vector2f& result) {
        sscanf(coord_line.data(), "vt %f %f", &result.x(), &result.y());
    }

    static void praseNormal(std::string_view normal_line, Vector3f& result) {
        sscanf(normal_line.data(), "vn %f %f %f", &result.x(), &result.y(), &result.z());
    }

    static void praseFace(std::string_view face_line, std::array<VertexInfo, 3>& result) {
        sscanf(face_line.data(), "f %d / %d / %d %d / %d / %d %d / %d / %d", 
                &result[0].vertex_index, &result[0].uv_index, &result[0].normal_index,
                &result[1].vertex_index, &result[1].uv_index, &result[1].normal_index,
                &result[2].vertex_index, &result[2].uv_index, &result[2].normal_index
        );
    }
};

#endif