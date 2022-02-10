#include <iostream>
#include <fstream>
#include <memory>
#include "material.hh"
#include "png_image.hh"

const Material Material::default_material = {
    "Default Material",
    Vector3f{ 1.0f, 1.0f, 1.0f },
    Vector3f{ 0.8f, 0.8f, 0.8f },
    Vector3f{ 0.5f, 0.5f, 0.5f },
    IllumMode::PHONG,
    nullptr,
    nullptr
};

Texture::Texture(): image_(nullptr) {}

bool Texture::loadTexture(const std::string& texture_file_name) {
    std::shared_ptr<PNGImage> image(new PNGImage());

    if(bool res = image->readPNG(texture_file_name); !res)
        return false;

    image_.swap(image);
    return true;
}