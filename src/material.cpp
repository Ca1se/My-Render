#include <iostream>
#include <fstream>
#include <memory>
#include "material.hpp"
#include "png_image.hpp"


Texture::Texture(): image_(nullptr) {}

bool Texture::loadTexture(const std::string& texture_file_name) {
    std::shared_ptr<PNGImage> image(new PNGImage());

    if(bool res = image->readPNG(texture_file_name); !res)
        return false;

    image_.swap(image);
    return true;
}