#pragma once

#include <stdint.h>


class ImagePiece
{
public:
    ImagePiece();
    ImagePiece(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height);
    uint16_t start_x() const;
    uint16_t start_y() const;
    uint16_t width() const;
    uint16_t height() const;
    uint8_t get_x_y_intensity(uint16_t x, uint16_t y) const;
private:
    uint8_t get_entry_byte(uint16_t x, uint16_t y) const;
    // uint8_t get_entry_nibble(uint16_t x, uint16_t y) const;
    uint16_t _start_x;
    uint16_t _start_y;
    uint16_t _width;
    uint16_t _height;
    uint8_t *_image_data;
};