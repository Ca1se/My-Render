#include <iostream>
#include <fstream>
#include <memory>
#include "material.hpp"
#include "png_image.hpp"


Texture::Texture(): m_image(nullptr) {}

bool Texture::loadTexture(const std::string& texture_file_name) {
    std::shared_ptr<PNGImage> image(new PNGImage());

    if(bool res = image->load(texture_file_name); !res)
        return false;

    m_image.swap(image);
    return true;
}
