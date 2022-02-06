#include <iostream>
#include <fstream>
#include "material.hh"

const Material Material::default_material = {
    "Default Material",
    Vector3f{ 1.0f, 1.0f, 1.0f },
    Vector3f{ 0.8f, 0.8f, 0.8f },
    Vector3f{ 0.5f, 0.5f, 0.5f },
    IllumMode::PHONG,
    nullptr,
    nullptr
};

Texture::Texture(const std::string& texture_file_name): good_(false) {
    // unfinished
}