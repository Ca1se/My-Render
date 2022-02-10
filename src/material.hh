#ifndef _MATERIAL_HH_
#define _MATERIAL_HH_

#include <cstdint>
#include <memory>
#include "math_utils.hh"


class Texture {
private:
    size_t width_;
    size_t height_;
    bool alpha_;
    std::shared_ptr<std::uint8_t[]> data_;

public:
    Texture();
    bool loadTexture(const std::string& texture_file_name);
    Vector<std::uint8_t, 3> get(int u, int v) const noexcept {
        // unfinished
    }
};

enum IllumMode {
    COLOR_ONLY = 0,
    DIFFUSE,
    PHONG
};

struct Material {
    static const Material default_material;

    std::string material_name;
    Vector3f ka;
    Vector3f kd;
    Vector3f ks;
    IllumMode illum_mode;
    std::shared_ptr<Texture> ambient_texture;
    std::shared_ptr<Texture> diffuse_texture;
};



#endif // _MATERIAL_HH_