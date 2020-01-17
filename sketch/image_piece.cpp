#include "image_piece.h"

#include <stdlib.h>
#include <assert.h>


#define BITS_PER_BYTE       8
#define ENTRIES_PER_BYTE    1
// #define BITS_PER_ENTRY      (BITS_PER_BYTE / ENTRIES_PER_BYTE)
// #define ENTRY_MASK          ((1 << BITS_PER_ENTRY) - 1)

// All data is encoded by rows (x)

ImagePiece::ImagePiece() :
    _start_x(0),
    _start_y(0),
    _width(0),
    _height(0),
    _current_index(0)
{
}

bool ImagePiece::init(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height)
{
    if (width * height > MAX_PIECE_BYTES)
    {
        return false;
    }
    _start_x = start_x;
    _start_y = start_y;
    _width = width;
    _height = height;
    _current_index = 0;
}

uint16_t ImagePiece::start_x() const
{
    return _start_x;
}

uint16_t ImagePiece::start_y() const
{
    return _start_y;
}

uint16_t ImagePiece::width() const
{
    return _width;
}

uint16_t ImagePiece::height() const
{
    return _height;
}

uint8_t ImagePiece::get_entry_byte(uint16_t x, uint16_t y) const
{
    uint16_t linear_entry = x * width() + y;
    return linear_entry / ENTRIES_PER_BYTE;
}

// uint8_t ImagePiece::get_entry_nibble(uint16_t x, uint16_t y) const
// {
//     uint16_t linear_entry = x * width() + y;
//     return linear_entry % ENTRIES_PER_BYTE;
// }

uint8_t ImagePiece::get_x_y_intensity(uint16_t x, uint16_t y) const
{
    uint16_t entry_byte = get_entry_byte(x, y);
    // uint16_t entry_nibble = get_entry_nibble(x, y);

    uint8_t byte = _image_data[entry_byte];
    // if (entry_nibble == 0)
    // {
    //     return byte & ENTRY_MASK;
    // }
    // else
    // {
    //     assert(entry_nibble == 1);
    //     return byte >> BITS_PER_ENTRY;
    // }
    return byte;
}

uint16_t ImagePiece::total_bytes() const
{
    return _width * _height;
}

bool ImagePiece::is_complete() const
{
    assert(_current_index <= total_bytes());

    return _current_index == total_bytes();
}

uint16_t ImagePiece::rx_buffer_bytes_remaining() const
{
    return total_bytes() - _current_index;
}

uint8_t *ImagePiece::get_data_rx_buffer(uint16_t bytes_to_add)
{
    if (is_complete())
    {
        return NULL;
    }

    if (_current_index + bytes_to_add > total_bytes())
    {
        return NULL;
    }

    return &_image_data[_current_index];
}

bool ImagePiece::report_bytes_added_to_rx_buffer(uint16_t bytes_added)
{
    // Caller overflowed the buffer
    assert(_current_index + bytes_added <= total_bytes());

    if (_current_index + bytes_added > total_bytes())
    {
        _current_index = total_bytes();
        return false;
    }

    _current_index += bytes_added;
}
