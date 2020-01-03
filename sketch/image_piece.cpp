#include "image_piece.h"

#include <stdlib.h>
#include <assert.h>

#define BITS_PER_BYTE       8
#define ENTRIES_PER_BYTE    2
#define BITS_PER_ENTRY      (BITS_PER_BYTE / ENTRIES_PER_BYTE)
#define ENTRY_MASK          ((1 << BITS_PER_ENTRY) - 1)

// All data is encoded by rows (x)

ImagePiece::ImagePiece() :
    _start_x(0),
    _start_y(0),
    _width(0),
    _height(0),
    _image_data(NULL)

{
}

ImagePiece::ImagePiece(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height) :
    _start_x(start_x),
    _start_y(start_y),
    _width(width),
    _height(height),
    _image_data(NULL)
{
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

uint8_t ImagePiece::get_entry_nibble(uint16_t x, uint16_t y) const
{
    uint16_t linear_entry = x * width() + y;
    return linear_entry % ENTRIES_PER_BYTE;
}

uint8_t ImagePiece::get_x_y_intensity(uint16_t x, uint16_t y) const
{
    uint16_t entry_byte = get_entry_byte(x, y);
    uint16_t entry_nibble = get_entry_nibble(x, y);

    uint8_t byte = _image_data[entry_byte];
    if (entry_nibble == 0)
    {
        return byte & ENTRY_MASK;
    }
    else
    {
        assert(entry_nibble == 1);
        return byte >> BITS_PER_ENTRY;
    }
}
