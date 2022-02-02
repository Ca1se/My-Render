#include <fstream>
#include <optional>
#include "model.hh"

void Model::loadModel(const std::string& obj_file_path) {

    std::string full_dir_path = model_dir_path_ + obj_file_path;
    std::string res;
    
    std::ifstream file(full_dir_path, std::ios::in | std::ios::binary | std::ios::ate);
    
    if(!file.is_open()) {
        throw std::runtime_error("Model::Model(): Cannot load file " + obj_file_path);
    }

    res.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*) res.data(), res.size());
    file.close();

    ObjPraser praser(res);
    praser.prase(models);
}

ObjPraser::ObjPraser(const std::string& obj_str) noexcept : 
        obj_str_(obj_str), now(0), pre(0) {}

size_t ObjPraser::prase(std::vector<Model>& result, bool recursed) {
    auto& model = result.emplace_back();

    while(hasNextLine()) {
        std::string_view line = nextLine();
        // o cube
        if(line[0] == 'o' && line[1] == ' ' && recursed) {
            return prase(result, true) + 1;
        // mtllib cube.mtl
        }else if(line[0] == 'm' && line == "mtllib") {

        }else if(line[0] == 'v') {
            // v 1.00 1.00 1.00
            if(line[1] == ' ') {
                auto& v = model.vertices_.emplace_back();
                praseVertex(line, v);
            // vt 1.00 1.00
            }else if(line[1] == 't') {
                auto& vt = model.uv_coords_.emplace_back();
                praseUVCoord(line, vt);
            // vn 1.00 1.00 1.00
            }else if(line[1] == 'n') {
                auto& vn = model.normals_.emplace_back();
                praseNormal(line, vn);
            }
        }else if(line[0] == 'f') {
            auto& f = model.faces_.emplace_back();
            praseFace(line, f);
        }
    }

    return 1;
}

std::string_view ObjPraser::nextLine() noexcept {
    if(now == obj_str_.size()) return std::string_view{};

    auto p = obj_str_.find('\n', now);
    
    if(p == std::string::npos) {
        pre = now;
        now = obj_str_.size();
        return std::string_view(obj_str_.substr(pre));
    }
    
    pre = now;
    now = p + 1;
    return std::string_view(obj_str_.substr(pre, p - pre));
}