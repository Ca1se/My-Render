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
#include <tuple>
#include <vector>


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

class PNGImage {
private:
    PNGImageHeader m_header;
    // std::shared_ptr<uint8_t[]> data_;
    std::vector<std::uint8_t> m_data;

public:
    bool load(const std::string& png_file_name);
    bool save(const std::string& png_file_name);

    size_t width() const noexcept { return m_header.width; }
    size_t height() const noexcept { return m_header.height; }

    // when the png image has no alpha, it will be: r g b r g b r g b ...
    // when the png image has alpha, it will be: r g b a r g b a ...
    std::uint8_t* data() noexcept { return m_data.data(); }

    const std::uint8_t* data() const noexcept { return m_data.data(); }

    size_t size() const noexcept { return (m_header.width * m_header.height); }

    std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>
    getColor(int x, int y) const noexcept {
        if(hasAlpha()) {
            int index = (y * width() + x) * 4;
            return std::make_tuple(
                m_data[index],
                m_data[index + 1],
                m_data[index + 2],
                m_data[index + 3]
            );
        }

        int index = (y * width() + x) * 3;
        return std::make_tuple(
            m_data[index],
            m_data[index + 1],
            m_data[index + 2],
            (std::uint8_t) 0xff
        );
    }

    bool hasAlpha() const noexcept { return (m_header.color_type == 6); }

    PNGImageHeader& header() noexcept { return m_header; }

    const PNGImageHeader& header() const noexcept { return m_header; }
};

#endif // _PNG_IMAGE_HPP_
