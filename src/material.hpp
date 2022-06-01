#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include <cstdint>
#include <memory>
#include <iostream>
#include "png_image.hpp"
#include "matrix.hpp"


class Texture {
private:
    std::shared_ptr<PNGImage> image_;

public:
    Texture();
    bool loadTexture(const std::string& texture_file_name);
    Vector<std::uint8_t, 3> sample(float u, float v) const noexcept {
        if(u < 0 || u > 1 || v < 0 || v > 1)
            return Vector<std::uint8_t, 3>{0, 0, 0};
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

#endif // _MATERIAL_HPP_