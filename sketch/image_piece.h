#pragma once

#include <stdint.h>


#define MAX_PIECE_BYTES 512

class ImagePiece
{
public:
    ImagePiece();
    bool init(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height);
    uint16_t start_x() const;
    uint16_t start_y() const;
    uint16_t width() const;
    uint16_t height() const;
    uint8_t get_x_y_intensity(uint16_t x, uint16_t y) const;
    bool is_complete() const;
    uint16_t rx_buffer_bytes_remaining() const;
    uint8_t *get_data_rx_buffer(uint16_t bytes_to_add);
    bool report_bytes_added_to_rx_buffer(uint16_t bytes_added);
    uint16_t total_bytes() const;
private:
    uint8_t get_entry_byte(uint16_t x, uint16_t y) const;
    // uint8_t get_entry_nibble(uint16_t x, uint16_t y) const;
    uint16_t _start_x;
    uint16_t _start_y;
    uint16_t _width;
    uint16_t _height;
    uint16_t _current_index;
    uint8_t _image_data[MAX_PIECE_BYTES];
};