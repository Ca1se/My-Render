#include <bits/types/FILE.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <zlib.h>
#include "png_image.hh"


static const std::uint64_t kPngSign = 0x89504e470d0a1a0a;

// most of computer cpu use little endian, 
// PNG file use big endian, so we need reverse endianness
inline constexpr std::uint64_t reverseEndian(std::uint64_t val) {
    const std::uint64_t fmt1 = 0x00ff00ff00ff00ff;
    const std::uint64_t fmt2 = 0x0000ffff0000ffff;
    const std::uint64_t fmt3 = 0x00000000ffffffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    val = ((val & fmt3) << 32) | ((val & (fmt3 << 32)) >> 32);
    return val;
}

inline constexpr std::uint32_t reverseEndian(std::uint32_t val) {
    const std::uint32_t fmt1 = 0x00ff00ff;
    const std::uint32_t fmt2 = 0x0000ffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    return val;
}

inline std::uint32_t calCRC32(PNGDataBlockType type, const std::uint8_t* data, size_t length) {
    std::uint8_t buf[4 + length];
    std::uint32_t type_val = reverseEndian((std::uint32_t) type);
    memcpy(buf, (const char*) &type_val, 4);
    memcpy(buf + 4, data, length);
    return crc32(0, buf, 4 + length);
}

PNGImage::PNGImage(): header_(), data_() {}

PNGImage::PNGImage(const PNGImage& image): 
        header_(image.header_), 
        data_(new std::uint8_t[sizeof(std::uint8_t) * image.size()]) {
    memcpy(data_.get(), image.data_.get(), sizeof(std::uint8_t) * image.size());
}

bool PNGImage::readPNG(const std::string& png_file_name) {
    std::ifstream file(png_file_name, std::ios::in | std::ios::binary);

    if(!file.is_open()) 
        return false;

    std::uint64_t sign;
    file.read((char*) &sign, 8);
    if(reverseEndian(sign) != kPngSign)
        return false;

    std::uint32_t length;
    std::uint32_t block_type;
    std::uint32_t crc;

    while(true) {
        file.read((char*) &length, 4);
        file.read((char*) &block_type, 4);
        length = reverseEndian(length);
        block_type = reverseEndian(block_type);
        
        if(block_type == IEND) {
            file.close();
            return true;
        }else if(block_type == IHDR) {
            file.read((char*) &header_, length);
            file.read((char*) &crc, 4);

            if(reverseEndian(crc) != calCRC32(IHDR, (std::uint8_t*) &header_, length))
                break;

            if(header_.color_type != TRUE_COLOR && header_.color_type != TRUE_COLOR_ALPHA)
                break;

            header_.width = reverseEndian(header_.width);
            header_.height = reverseEndian(header_.height);
        }else if(block_type == IDAT){
            std::shared_ptr<std::uint8_t[]> buf(new std::uint8_t[length]);
            file.read((char*) buf.get(), length);
            file.read((char*) &crc, 4);

            if(reverseEndian(crc) != calCRC32(IDAT, (std::uint8_t*) buf.get(), length))
                break;

            // unfinished
        }else {
            file.seekg(length + 4, std::ios::cur);
        }
    }

    file.close();
    return false;
}