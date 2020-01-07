#include "image_receiver.h"

#include "serial_interface.h"

#include <arpa/inet.h>
#include <stdio.h>
#define LOG printf
#include <unistd.h>
#define delay   usleep

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
  |--- start piece       -->|   Image piece header
  |                         |
  |<-- start piece ack   ---|
  |                         |
  |--- image data        -->|   RX buffer size bytes no header/metadata
  |                         |
  |<-- image data ack    ---|   Repeat until all data specified in header has been sent
  |                         |
  |<-- image piece ack   ---|   Success or failure based on data received without timeout and matching CRC
  |                         |
*/

#define BURNER_MAGIC    0x4652 // 'FR' big endian

enum Op
{
    REGISTER_REQ_OP = 0,
    REGISTER_ACK_OP = 1,
    START_PIECE_REQ_OP = 2,
    START_PIECE_ACK_OP = 3,
    IMG_DATA_REQ_OP = 4,
    IMG_DATA_ACK_OP = 5,
    COMPLETE_PIECE_ACK_OP = 6
};

struct req_header
{
    uint16_t magic;
    uint16_t op;
    void deswizzle()
    {
        magic = ntohs(magic);
        op = ntohs(op);
    }
    bool validate()
    {
        return magic == BURNER_MAGIC && (op == REGISTER_REQ_OP || op == START_PIECE_REQ_OP || op == IMG_DATA_REQ_OP);
    }
};

struct ack_header
{
    uint16_t magic;
    uint16_t op;
    ack_header(Op opcode) :
        magic(BURNER_MAGIC),
        op(opcode)
    {
    }
    void swizzle()
    {
        magic = htons(magic);
        op = htons(op);
    }
};

struct register_req : public req_header
{
    bool validate()
    {
        if (!req_header::validate())
        {
            return false;
        }

        return op == REGISTER_REQ_OP;
    }
};

struct register_ack : public ack_header
{
    uint16_t rx_buffer_size;
    uint16_t max_dim;
    register_ack(uint16_t rx_buffer_size, uint16_t max_dim) :
        ack_header(REGISTER_ACK_OP),
        rx_buffer_size(rx_buffer_size),
        max_dim(max_dim)
    {
    }
    void swizzle()
    {
        ack_header::swizzle();
        rx_buffer_size = htons(rx_buffer_size);
        max_dim = htons(max_dim);
    }
};

struct image_piece_header
{
    uint16_t start_x;
    uint16_t start_y;
    uint16_t width;
    uint16_t height;
    uint16_t header_crc;
    uint16_t image_data_crc;
    void deswizzle()
    {
        start_x = ntohs(start_x);
        start_y = ntohs(start_y);
        width = ntohs(width);
        height = ntohs(height);
        header_crc = ntohs(header_crc);
        image_data_crc = ntohs(image_data_crc);
    }
    bool validate()
    {
        return true;
    }
};

struct start_req : public req_header
{
    image_piece_header image_piece;
    bool validate()
    {
        if (!req_header::validate())
        {
            return false;
        }

        if (op != START_PIECE_REQ_OP)
        {
            return false;
        }

        return image_piece.validate();
    }
    void deswizzle()
    {
        req_header::deswizzle();
        image_piece.deswizzle();
    }
};

struct start_ack : public ack_header
{
    start_ack() :
        ack_header(START_PIECE_ACK_OP)
    {
    }
};

void ImageReceiver::start()
{
    register_req req;

    for(;;)
    {
        LOG("Waiting for controller registration");
        while(_serial->available() < sizeof(req))
        {
            // TODO: delay with tinmeout (maybe no timeout).
            delay(10);
        }

        LOG("Reading registration request...\n");
        // TODO: verify assumption that available bytes will always be read all at once.
        size_t bytes_read = _serial->readBytes(reinterpret_cast<uint8_t *>(&req), sizeof(req));
        if (bytes_read != sizeof(req))
        {
            LOG("I/O error reading registration. Restarting...\n");
            continue;  // IO error
        }

        LOG("Received registration. Validating...\n");
        req.deswizzle();
        if (!req.validate())
        {
            // TODO: Log this
            LOG("Registration not valid. Restarting...\n");
            continue;
        }

        LOG("Registration valid. Sending ACK.\n");
        // Send the register ack
        // TODO: get from setial interface and burner
        register_ack ack(64, 150);
        ack.swizzle();
        size_t bytes_sent = _serial->writeBytes(reinterpret_cast<const uint8_t *>(&ack), sizeof(ack));
        if (bytes_sent != sizeof(ack))
        {
            LOG("I/O error sending registration ACK. Restarting...\n");
            continue;
        }

        // registration complete
        LOG("Registration complete.\n");
        break;
    }
}

bool ImageReceiver::done() const
{
    return true;
}

void ImageReceiver::get_next_image_piece(ImagePiece *&piece)
{
    image_piece_header header;

    // Wait for data to arrive
    while(_serial->available() < sizeof(header))
    {
        // TODO: delay with tinmeout.
        delay(10);
    }

    _serial->readBytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

}
