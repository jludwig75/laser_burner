#include "image_receiver.h"

#include "serial_interface.h"



/*
Initial start up
----------------
Controller                Burner
  |--- register          -->|
  |                         |
  |<-- register ack      ---|   RX buffer size
  |                         |


For each image piece:
----------------
Controller                Burner
  |--- start image piece -->|   Image piece header
  |                         |
  |<-- start ack         ---|
  |                         |
  |--- image data        -->|   RX buffer size bytes no header/metadata
  |                         |
  |<-- image data ack    ---|   Repeat until all data specified in header has been sent
  |                         |
  |<-- image piece ack   ---|   Success or failure based on data received without timeout and matching CRC
  |                         |
*/

struct image_piece_header
{
    uint16_t magic;
    uint16_t op;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t width;
    uint16_t height;
    uint16_t header_crc;
    uint16_t image_data_crc;
};

struct image_piece_header_ack
{
    uint32_t magic;
};

bool ImageReceiver::done() const
{
    return false;
}

void ImageReceiver::get_next_image_piece(ImagePiece *&piece)
{
    image_piece_header header;

    // Wait for data to arrive
    while(_serial->available() < sizeof(header))
    {
        // TODO: delay with tinmeout.
    }

    _serial->readBytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

}
