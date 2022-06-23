#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include <cstdint>
#include <memory>
#include <iostream>
#include "png_image.hpp"
#include "matrix.hpp"


class Texture {
private:
    std::shared_ptr<PNGImage> m_image;

public:
    Texture();
    bool loadTexture(const std::string& texture_file_name);
    Vector<std::uint8_t, 3> sample(float u, float v) const noexcept {
        if(u < 0 || u > 1 || v < 0 || v > 1)
            return Vector<std::uint8_t, 3>{ 0, 0, 0 };
        int img_u = u * m_image->width();
        int img_v = (1 - v) * m_image->height();
        auto [r, g, b, a] = m_image->getColor(img_u, img_v);
        return Vector<std::uint8_t, 3>{ r, g, b };
    }

};


#endif // _MATERIAL_HPP_
