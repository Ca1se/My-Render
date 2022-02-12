#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <algorithm>
#include <cstdio>
#include <string_view>
#include <optional>
#include <vector>
#include <string>
#include "material.hpp"
#include "matrix.hpp"


struct VertexInfo {
    int vertex_index;
    int normal_index;
    int uv_index;
};

struct Face {
    int material_index;
    VertexInfo vertex[3];
};

struct Model {
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uv_coords;
    // vectices' index, only support triangles
    std::vector<Face> faces;
};

class ObjLoader {
private:
    class Lines {
    private:
        const std::string& str_;
        size_t now_;
        size_t pre_;
    
    public:
        Lines(const std::string& str): str_(str), now_(0), pre_(0) {}

    public:
        bool hasNextLine() const noexcept { return now_ != str_.size(); }
        std::string_view nextLine() noexcept {
            if(now_ == str_.size()) return std::string_view{};

            auto p = str_.find('\n', now_);
            
            if(p == std::string::npos) {
                pre_ = now_;
                now_ = str_.size();
                return std::string_view(str_.c_str() + pre_);
            }
            
            pre_ = now_;
            now_ = p + 1;
            return std::string_view(str_.c_str() + pre_, p - pre_);
        }
    };

private:
    std::vector<Model> loaded_models_;
    std::vector<Material> loaded_materials_;

public:
    ObjLoader();

public:
    void loadModel(const std::string& obj_file_name);
    void loadMaterial(const std::string& mtl_file_name);

    const std::vector<Model>& loaded_models() const { return loaded_models_; }
    const std::vector<Material>& loaded_materials() const { return loaded_materials_; }


private:
    static void praseVertex(std::string_view vector_line, std::vector<Vector3f>& vectices) {
        auto& result = vectices.emplace_back();
        sscanf(vector_line.data(), "v %f %f %f", &result.x(), &result.y(), &result.z());
    }

    static void praseUVCoord(std::string_view coord_line, std::vector<Vector2f>& coords) {
        auto& result = coords.emplace_back();
        sscanf(coord_line.data(), "vt %f %f", &result.x(), &result.y());
    }

    static void praseNormal(std::string_view normal_line, std::vector<Vector3f>& normals) {
        auto& result = normals.emplace_back();
        sscanf(normal_line.data(), "vn %f %f %f", &result.x(), &result.y(), &result.z());
    }

    static void praseFace(std::string_view face_line, std::vector<Face>& faces, int material_index) {
        auto& face1 = faces.emplace_back();
        face1.material_index = material_index;
        int cnt = std::count(face_line.begin(), face_line.end(), '/');
        if(cnt == 6) {
            sscanf(face_line.data(), "f %d / %d / %d %d / %d / %d %d / %d / %d", 
                &face1.vertex[0].vertex_index, &face1.vertex[0].uv_index, &face1.vertex[0].normal_index,
                &face1.vertex[1].vertex_index, &face1.vertex[1].uv_index, &face1.vertex[1].normal_index,
                &face1.vertex[2].vertex_index, &face1.vertex[2].uv_index, &face1.vertex[2].normal_index
            );
            for(int i = 0; i < 3; i++) {
                face1.vertex[i].vertex_index--;
                face1.vertex[i].uv_index--;
                face1.vertex[i].normal_index--;
            }
        }else if(cnt == 8) {
            int tmp[4][3];
            sscanf(face_line.data(), "f %d / %d / %d %d / %d / %d %d / %d / %d %d / %d / %d",
                &tmp[0][0], &tmp[0][1], &tmp[0][2],
                &tmp[1][0], &tmp[1][1], &tmp[1][2],
                &tmp[2][0], &tmp[2][1], &tmp[2][2],
                &tmp[3][0], &tmp[3][1], &tmp[3][2]
            );
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 3; j++) {
                    tmp[i][j]--;
                }
            }
            face1.vertex[0].vertex_index = tmp[0][0], face1.vertex[0].uv_index = tmp[0][1], face1.vertex[0].normal_index = tmp[0][2];
            face1.vertex[1].vertex_index = tmp[1][0], face1.vertex[1].uv_index = tmp[1][1], face1.vertex[1].normal_index = tmp[1][2];
            face1.vertex[2].vertex_index = tmp[2][0], face1.vertex[2].uv_index = tmp[2][1], face1.vertex[2].normal_index = tmp[2][2];

            auto& face2 = faces.emplace_back();
            face2.material_index = material_index;
            face2.vertex[0].vertex_index = tmp[0][0], face2.vertex[0].uv_index = tmp[0][1], face2.vertex[0].normal_index = tmp[0][2];
            face2.vertex[1].vertex_index = tmp[2][0], face2.vertex[1].uv_index = tmp[2][1], face2.vertex[1].normal_index = tmp[2][2];
            face2.vertex[2].vertex_index = tmp[3][0], face2.vertex[2].uv_index = tmp[3][1], face2.vertex[2].normal_index = tmp[3][2];
        }
    }
};

#endif