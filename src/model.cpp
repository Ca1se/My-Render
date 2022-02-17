#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include "material.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "png_image.hpp"


std::shared_ptr<Model> ObjLoader::loadModel(const std::string& obj_file_name) {
    std::string obj_str;
    std::ifstream file(obj_file_name, std::ios::in | std::ios::binary | std::ios::ate);
    
    if(!file.is_open()) {
        std::cerr << "Load Obj failed: ObjLoader::loadModel(): Cannot load file " + obj_file_name << "\n";
        return nullptr;
    }

    obj_str.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*) obj_str.data(), obj_str.size());
    file.close();

    Lines lines(obj_str);

    std::shared_ptr<Model> model(new Model);

    while(lines.hasNextLine()) {
        std::string_view line = lines.nextLine();
        if(line[0] == 'v') {
            // v 1.00 1.00 1.00
            if(line[1] == ' ') {
                praseVertex(line, model->vertices);
            // vt 1.00 1.00
            }else if(line[1] == 't') {
                praseUVCoord(line, model->uv_coords);
            // vn 1.00 1.00 1.00
            }else if(line[1] == 'n') {
                praseNormal(line, model->normals);
            }
        // face v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
        }else if(line[0] == 'f' && line[1] == ' ') {
            praseFace(line, model->faces);
        }
    }
    return model;
}

std::shared_ptr<Texture> ObjLoader::loadTexture(const std::string& texture_file_name) {
    std::shared_ptr<Texture> texture(new Texture);
    if(bool res = texture->loadTexture(texture_file_name); !res) {
        std::cerr << "Load Texture failed: ObjLoader::loadTexture(): Cannot load file " + texture_file_name << "\n";
        return nullptr;
    }

    return texture;
}