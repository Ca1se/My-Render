#include <iostream>
#include <fstream>
#include "material.hh"

constexpr uint64_t reverseEndian(uint64_t val) {
    const uint64_t fmt1 = 0x00ff00ff00ff00ff;
    const uint64_t fmt2 = 0x0000ffff0000ffff;
    const uint64_t fmt3 = 0x00000000ffffffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    val = ((val & fmt3) << 32) | ((val & (fmt3 << 32)) >> 32);
    return val;
}

constexpr uint32_t reverseEndian(uint32_t val) {
    const uint32_t fmt1 = 0x00ff00ff;
    const uint32_t fmt2 = 0x0000ffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    return val;
}

Texture::Texture(const std::string& texture_file_name): good_(false) {
    std::ifstream file(texture_file_name, std::ios::in | std::ios::binary | std::ios::ate);

    if(!file.is_open()) {
        std::cerr << "Texture load failed: Texture::Texture(): Cannot find file " + texture_file_name << "\n";
        return;
    }

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    uint64_t sign;
    file.read((char*) &sign, 8);

    if(reverseEndian(sign) != kPngSign) {
        std::cerr << "Texture load failed: Texture::Texture(): Only support PNG texture\n";
    }
}