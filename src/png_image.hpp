// Author: Wu Zihao
// Github: https://github.com/Ca1se
//
// This a simple PNG image library, it can read PNG image.
// This library currently only support read true color image (24 or 32 bits).
// This library require C++17 and zlib

#ifndef _PNG_IMAGE_HPP_
#define _PNG_IMAGE_HPP_

#include <cstdint>
#include <memory>
#include <string>

enum PNGDataBlockType {
    IHDR = (int('I') << 24) | (int('H') << 16) | (int('D') << 8) | int('R'),
    PLTE = (int('P') << 24) | (int('L') << 16) | (int('T') << 8) | int('E'),
    IDAT = (int('I') << 24) | (int('D') << 16) | (int('A') << 8) | int('T'),
    IEND = (int('I') << 24) | (int('E') << 16) | (int('N') << 8) | int('D')
};

enum PNGImageColorType {
    GRAY_SCALE = 0,
    TRUE_COLOR = 2,
    INDEXED_COLOR = 3,
    GRAY_SCALE_ALPHA = 4,
    TRUE_COLOR_ALPHA = 6
};

struct PNGImageHeader {
    std::uint32_t width;
    std::uint32_t height;
    std::uint8_t bit_depth;
    std::uint8_t color_type;
    std::uint8_t compression_method;
    std::uint8_t filter_method;
    std::uint8_t interlace_method;
};

struct PNGColor {
    std::uint8_t rgba[4];

    PNGColor() = default;
    PNGColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255): 
            rgba{r, g, b, a} {}
    std::uint8_t& operator[] (size_t index) noexcept {
        return rgba[index];
    }
    uint8_t operator[] (size_t index) const noexcept {
        return rgba[index];
    }
};

class PNGImage {
private:
    PNGImageHeader header_;
    std::shared_ptr<uint8_t[]> data_;

public:
    PNGImage();
    PNGImage(const PNGImage& image);

public:
    bool readPNG(const std::string& png_file_name);
    bool generatePNG(const std::string& png_file_name);

    size_t width() const noexcept { return header_.width; }
    size_t height() const noexcept { return header_.height; }

    // when the png image has no alpha, it will be: r g b r g b r g b ...
    // when the png image has alpha, it will be: r g b a r g b a ...
    std::uint8_t* data() const noexcept { return data_.get(); }
    std::shared_ptr<std::uint8_t[]> sharedData() const noexcept { return data_; }

    size_t size() const noexcept { return header_.width * header_.height; }

    PNGColor getColor(int x, int y) const noexcept {
        if(hasAlpha()) {
            int index = (y * width() + x) * 4;
            return PNGColor(
                data_[index], 
                data_[index + 1], 
                data_[index + 2], 
                data_[index + 3]
            );
        }

        int index = (y * width() + x) * 3;
        return PNGColor(
            data_[index],
            data_[index + 1],
            data_[index + 2]
        );
    }

    bool hasAlpha() const noexcept { return (header_.color_type == 6); }

    PNGImageHeader& header() noexcept { return header_; }
};

#endif // _PNG_IMAGE_HPP_