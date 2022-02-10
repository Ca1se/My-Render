#include <iostream>
#include <fstream>
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

Texture::Texture(): width_(0), height_(0), data_(nullptr), alpha_(false) {}

bool Texture::loadTexture(const std::string& texture_file_name) {
    PNGImage image;

    if(bool res = image.readPNG(texture_file_name); !res)
        return false;

    width_ = image.width();
    height_ = image.height();
    alpha_ = image.hasAlpha();
    data_ = image.sharedData();

    return true;
}