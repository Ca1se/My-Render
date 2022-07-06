#include <cstdint>
#include <iostream>
#include <cstring>
#include <fstream>
#include <memory>
#include <zconf.h>
#include <zlib.h>
#include "png_image.hpp"


static constexpr std::uint64_t kPngSign = 0x89504e470d0a1a0a;

// most of computer cpu use little endian, 
// PNG file use big endian, so we need reverse endianness
constexpr inline std::uint64_t reverseEndian(std::uint64_t val) {
    constexpr std::uint64_t fmt1 = 0x00ff00ff00ff00ff;
    constexpr std::uint64_t fmt2 = 0x0000ffff0000ffff;
    constexpr std::uint64_t fmt3 = 0x00000000ffffffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    val = ((val & fmt3) << 32) | ((val & (fmt3 << 32)) >> 32);
    return val;
}

constexpr inline std::uint32_t reverseEndian(std::uint32_t val) {
    constexpr std::uint32_t fmt1 = 0x00ff00ff;
    constexpr std::uint32_t fmt2 = 0x0000ffff;

    val = ((val & fmt1) << 8) | ((val & (fmt1 << 8)) >> 8);
    val = ((val & fmt2) << 16) | ((val & (fmt2 << 16)) >> 16);
    return val;
}

inline std::uint32_t calCRC32(PNGDataBlockType type, const std::uint8_t* data, size_t length) {
    std::vector<std::uint8_t> buf(4 + length);
    std::uint32_t type_val = reverseEndian((std::uint32_t) type);
    memcpy(buf.data(), (const char*) &type_val, 4);
    memcpy(buf.data() + 4, data, length);
    return crc32(0, buf.data(), 4 + length);
}

constexpr inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c) {
    int base = a + b - c;
    int pa = abs(base - (int) a);
    int pb = abs(base - (int) b);
    int pc = abs(base - (int) c);
    if(pa <= pb && pa <= pc) return a;
    else if(pb <= pc) return b;
    else return c;
}

bool PNGImage::load(const std::string& png_file_name) {
    std::ifstream file(png_file_name, std::ios::in | std::ios::binary);
    std::vector<std::uint8_t> compressed_data;
    std::uint32_t data_index = 0;
    
    if(!file.is_open()) 
        goto failed;

    std::uint64_t sign;
    file.read((char*) &sign, 8);
    if(reverseEndian(sign) != kPngSign)
        goto failed;

    std::uint32_t length;
    std::uint32_t block_type;
    std::uint32_t crc;

    while(true) {
        if(!file.read((char*) &length, 4))
            goto failed;

        file.read((char*) &block_type, 4);
        length = reverseEndian(length);
        block_type = reverseEndian(block_type);
        
        if(block_type == IEND) {
            std::uint32_t bpp = (m_header.color_type == TRUE_COLOR ? 3 : 4);
            std::uint32_t data_width = m_header.width * bpp;
            std::uint32_t data_height = m_header.height;
            uLongf len = (data_width + 1) * data_height;
            std::vector<std::uint8_t> filtered_data(len);
            
            std::uint32_t loop_count = 0;
            do {
                std::uint32_t res = uncompress(filtered_data.data(), &len, compressed_data.data(), data_index);
                if(res == Z_OK) {
                    m_data.resize(data_width * data_height);
                    for(std::uint32_t i = 0; i < data_height; i++) {
                        std::uint32_t data_begin = i * data_width;
                        std::uint32_t filter_begin = i * (data_width + 1);
                        memcpy(m_data.data() + data_begin, filtered_data.data() + filter_begin + 1, data_width);
                        switch(filtered_data[filter_begin]) {
                            case 0x00:
                                break;
                            case 0x01: {
                                for(std::uint32_t k = data_begin + bpp; k < data_begin + data_width; k++)
                                    m_data[k] = m_data[k] + m_data[k - bpp];
                                break;
                            }
                            case 0x02: {
                                if(i == 0) goto failed;
                                for(std::uint32_t k = data_begin; k < data_begin + data_width; k++)
                                    m_data[k] = m_data[k] + m_data[k - data_width];
                                break;
                            }
                            case 0x03: {
                                if(i == 0) goto failed;
                                for(std::uint32_t k = data_begin; k < data_begin + bpp; k++) {
                                    m_data[k] = m_data[k] + m_data[k - data_width] / 2;
                                }
                                for(std::uint32_t k = data_begin + bpp; k < data_begin + data_width; k++)
                                    m_data[k] = m_data[k] + (std::uint8_t) (((int) m_data[k - bpp] + (int) m_data[k - data_width]) / 2);
                                break;
                            }
                            case 0x04: {
                                if(i == 0) goto failed;
                                for(std::uint32_t k = data_begin; k < data_begin + bpp; k++) {
                                    m_data[k] = m_data[k] + m_data[k - data_width];
                                }
                                for(std::uint32_t k = data_begin + bpp; k < data_begin + data_width; k++) {
                                    m_data[k] = m_data[k] + paethPredictor(m_data[k - bpp], m_data[k - data_width], m_data[k - data_width - bpp]);
                                }
                                break;
                            }
                            default:
                                goto failed;
                        }
                    }
                    break;
                }else if(res == Z_MEM_ERROR) {
                    if(++loop_count < 10)
                        continue;
                    else
                        goto failed;
                }else {
                    goto failed;
                }
            }while(true);
            file.close();
            return true;
        }else if(block_type == IHDR) {
            file.read((char*) &m_header, length);
            file.read((char*) &crc, 4);

            if(reverseEndian(crc) != calCRC32(IHDR, (std::uint8_t*) &m_header, length))
                goto failed;

            if(m_header.color_type != TRUE_COLOR && m_header.color_type != TRUE_COLOR_ALPHA)
                goto failed;

            m_header.width = reverseEndian(m_header.width);
            m_header.height = reverseEndian(m_header.height);
            compressed_data.resize((m_header.width * 4 + 1) * m_header.height);
        }else if(block_type == IDAT){
            if(compressed_data.empty()) goto failed;
            std::vector<std::uint8_t> buf(length);
            file.read((char*) buf.data(), length);
            file.read((char*) &crc, 4);

            if(reverseEndian(crc) != calCRC32(IDAT, (std::uint8_t*) buf.data(), length))
                goto failed;

            memcpy(compressed_data.data() + data_index, buf.data(), length);
            data_index += length;

            
        }else { 
            file.seekg(length + 4, std::ios::cur);
        }
    }

failed:
    file.close();
    return false;
}

bool PNGImage::save(const std::string& png_file_name) {
    static constexpr std::uint8_t iend[12] = {0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};

    std::ofstream file(png_file_name, std::ios::out | std::ios::binary | std::ios::trunc);

    auto writeBlock = [&file](PNGDataBlockType type, const uint8_t* data, std::uint32_t length) {
        std::uint32_t dlen = reverseEndian(length);
        std::uint32_t ascii = reverseEndian((std::uint32_t) type);
        std::uint32_t crc = reverseEndian(calCRC32(type, data, length));
        
        file.write((char*) &dlen, 4);
        file.write((char*) &ascii, 4);
        file.write((char*) data, length);
        file.write((char*) &crc, 4);
    };

    if(!file.is_open()) {
        file.close();
        return false;
    }

    std::uint64_t sign = reverseEndian(kPngSign);
    file.write((char*) &sign, 8);

    std::uint32_t length;
    std::uint32_t ascii;
    std::uint32_t crc;

    // IHDR
    PNGImageHeader header = m_header;
    header.width = reverseEndian(header.width);
    header.height = reverseEndian(header.height);

    writeBlock(IHDR, (std::uint8_t*) &header, 13);

    // IDAT
    int bpp = (m_header.color_type == TRUE_COLOR ? 3 : 4);
    uLongf len = (m_header.width * bpp + 1) * m_header.height;
    std::vector<std::uint8_t> buf(len);
    for(std::uint32_t i = 0; i < m_header.height; i++) {
        std::uint32_t buf_begin = i * (m_header.width * bpp + 1);
        std::uint32_t data_begin = i * m_header.width * bpp;
        buf[buf_begin] = 0x00;
        memcpy(buf.data() + buf_begin + 1, m_data.data() + data_begin, m_header.width * bpp);
    }

    std::vector<std::uint8_t> compressed_data(len);
    compress(compressed_data.data(), &len, buf.data(), len);


    writeBlock(IDAT, compressed_data.data(), len);

    // IEND
    file.write((char*) iend, 12);
    file.close();

    return true;
}
