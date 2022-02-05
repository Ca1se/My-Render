#include <fstream>
#include <iostream>
#include "material.hh"
#include "math_utils.hh"
#include "model.hh"

ObjLoader::ObjLoader(): loaded_models_(), loaded_materials_{ Material::default_material } {}

void ObjLoader::loadModel(const std::string& obj_file_path) {
    std::string obj_str;
    std::ifstream file(obj_file_path, std::ios::in | std::ios::binary | std::ios::ate);
    
    if(!file.is_open()) {
        std::cerr << "Load Obj failed: ObjLoader::loadModel(): Cannot find file " + obj_file_path << "\n";
        return;
    }

    obj_str.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*) obj_str.data(), obj_str.size());
    file.close();

    Lines lines(obj_str);

    Model* model = &loaded_models_.emplace_back();
    bool first_model = true;
    int use_material = 0;
    char buf[50];

    while(lines.hasNextLine()) {
        std::string_view line = lines.nextLine();
        // o cube
        if(line[0] == 'o' && line[1] == ' ') {
            if(first_model) {
                first_model = false;
            }else {
                model = &loaded_models_.emplace_back();
            }
        // mtllib cube.mtl
        }else if(line[0] == 'm' && line.substr(0, 6) == "mtllib") {
            sscanf(line.data(), "mtllib %s", buf);
            loadMaterial(buf);
        // usemtl Material
        }else if(line[0] == 'u' && line.substr(0, 6) == "usemtl") {
            sscanf(line.data(), "usemtl %s", buf);
            std::string name(buf);
            for(int i = 0; i < loaded_materials_.size(); i++) {
                if(loaded_materials_[i].material_name == name) {
                    use_material = i;
                    break;
                }
            }
        }else if(line[0] == 'v') {
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
            praseFace(line, model->faces, use_material);
        }
    }
}

void ObjLoader::loadMaterial(const std::string& mtl_file_name) {
    std::string mtl_str;
    std::ifstream file(mtl_file_name, std::ios::in | std::ios::binary | std::ios::ate);
    
    if(!file.is_open()) {
        std::cerr << "Load Material failed: ObjLoader::loadMaterial(): Cannot find file " + mtl_file_name << "\n";
        return;
    }

    mtl_str.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read((char*) mtl_str.data(), mtl_str.size());
    file.close();

    Lines lines(mtl_str);
    char buf[50];

    Material* material = nullptr;

    while(lines.hasNextLine()) {
        std::string_view line = lines.nextLine();

        // newmtl material_name
        if(line[0] == 'n' && line.substr(0, 6) == "newmtl") {
            material = &loaded_materials_.emplace_back();
            sscanf(line.data(), "newmtl %s", buf);
            material->material_name = std::string(buf);
        }else if(line[0] == 'K') {
            // Ka r g b
            if(line[1] == 'a') {
                sscanf(line.data(), "Ka %f %f %f", &material->ka.x(), 
                        &material->ka.y(), &material->ka.z());
            // Kd r g b
            }else if(line[1] == 'd') {
                sscanf(line.data(), "Kd %f %f %f", &material->kd.x(), 
                        &material->kd.y(), &material->kd.z());
            // Ks r g b
            }else if(line[1] == 's') {
                sscanf(line.data(), "Ks %f %f %f", &material->ks.x(), 
                        &material->ks.y(), &material->ks.z());
            }
        // illum illumno, 0 for color on and ambient off, 1 for color on and ambient on, 2 for highlight on
        }else if(line[0] == 'i' && line.substr(0, 5) == "illum") {
            sscanf(line.data(), "illum %d", &material->illum_mode);
        }else if(line[0] == 'm') {
            // map_Ka texture path
            if(line.substr(0, 6) == "map_Ka") {
                sscanf(line.data(), "map_Ka %s", buf);
                Texture* tex = new Texture(buf);
                if(!tex->good()) {
                    delete tex;
                    continue;
                }
                material->ambient_texture.reset(tex);
            // map_Kd texture path
            }else if(line.substr(0, 6) == "map_Kd") {
                sscanf(line.data(), "map_Kd %s", buf);
                Texture* tex = new Texture(buf);
                if(!tex->good()) {
                    delete tex;
                    continue;
                }
                material->diffuse_texture.reset(tex);
            }
        }
    }
}