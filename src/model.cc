#include <fstream>
#include "model.hh"

ObjLoader& ObjLoader::load(const std::string& obj_file_path) {
    now = pre = 0;

    std::ifstream file(obj_file_path, std::ios::in | std::ios::binary | std::ios::ate);
    
    if(!file.is_open()) {
        throw std::runtime_error("Model::Model(): Cannot load file " + obj_file_path);
    }

    obj_str_.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*) obj_str_.data(), obj_str_.size());
    file.close();

    praseObj();
}

void ObjLoader::praseObj() {
    Model* model = &loaded_models_.emplace_back();
    bool first_model = true;

    while(hasNextLine()) {
        std::string_view line = nextLine();
        // o cube
        if(line[0] == 'o' && line[1] == ' ') {
            if(first_model) {
                first_model = false;
            }else {
                model = &loaded_models_.emplace_back();
            }
        // mtllib cube.mtl
        }else if(line[0] == 'm' && line == "mtllib") {

        }else if(line[0] == 'v') {
            // v 1.00 1.00 1.00
            if(line[1] == ' ') {
                auto& v = model->vertices_.emplace_back();
                praseVertex(line, v);
            // vt 1.00 1.00
            }else if(line[1] == 't') {
                auto& vt = model->uv_coords_.emplace_back();
                praseUVCoord(line, vt);
            // vn 1.00 1.00 1.00
            }else if(line[1] == 'n') {
                auto& vn = model->normals_.emplace_back();
                praseNormal(line, vn);
            }
        }else if(line[0] == 'f') {
            auto& f = model->faces_.emplace_back();
            praseFace(line, f);
        }
    }
}

std::string_view ObjLoader::nextLine() noexcept {
    if(now == obj_str_.size()) return std::string_view{};

    auto p = obj_str_.find('\n', now);
    
    if(p == std::string::npos) {
        pre = now;
        now = obj_str_.size();
        return std::string_view(obj_str_.c_str() + pre);
    }
    
    pre = now;
    now = p + 1;
    return std::string_view(obj_str_.c_str() + pre, p - pre);
}