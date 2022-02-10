#ifndef _MATERIAL_HH_
#define _MATERIAL_HH_

#include <cstdint>
#include <memory>
#include "png_image.hh"
#include "math_utils.hh"


class Texture {
private:
    std::shared_ptr<PNGImage> image_;

public:
    Texture();
    bool loadTexture(const std::string& texture_file_name);
    Vector<std::uint8_t, 3> sample(float u, float v) const noexcept {
        int img_u = u * image_->width();
        int img_v = (1 - v) * image_->height();
        auto color = image_->getColor(img_u, img_v);
        return Vector<std::uint8_t, 3>{color[0], color[1], color[2]};
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