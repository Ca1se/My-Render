#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include <vector>
#include <string>
#include "math_utils.hpp"

class StringPraser;

struct VertexInfo {
    int vertex_index;
    int normal_index;
    int uv_index;
};

class Model {
private:
    static std::string model_dir_path_;
    static std::vector<Model> models_;

    std::vector<Vector3f> vertices_;
    std::vector<Vector3f> normals_;
    std::vector<Vector2f> uv_coord_;

    // vectices' index
    std::vector<std::array<VertexInfo, 3>> faces_;

public:
    static void setModelDirPath(const std::string& path) {
        model_dir_path_ = path;
    }

    static void loadModel(const std::string& obj_file_path);

private:
    static void praseObj(StringPraser& obj_str, bool recurse);
};

inline std::string Model::model_dir_path_ = "./model/";

#endif