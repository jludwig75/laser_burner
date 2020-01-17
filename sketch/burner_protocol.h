#pragma once

#include <stdint.h>
#include <arpa/inet.h>



/*
Initial start up
----------------
Controller                Burner
  |--- inquiry           -->|
  |                         |
  |<-- inquiry ack       ---|   RX buffer size
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
*/

#define BURNER_MAGIC    0x4652 // 'FR' big endian

enum Op
{
    UNKNOWN_OP = 0,

    INQUIRY_REQ_OP = 1,
    INQUIRY_ACK_OP = 2,

    START_PIECE_REQ_OP = 3,
    START_PIECE_ACK_OP = 4,

    IMG_DATA_REQ_OP = 5,
    IMG_DATA_ACK_OP = 6
};

enum AckStatus
{
    ACK_SATUS_SUCCESS = 0,
    ACK_SATUS_BAD_MAGIC = 1,
    ACK_SATUS_INVALID_REQUEST_OP = 2,
    ACK_SATUS_INVALID_REQUEST = 3,
    ACK_SATUS_INVALID_PARAMETER = 4,
    ACK_SATUS_NOT_IMPLEMENTED = 5,
    ACK_STATUS_IO_ERROR = 6,
    ACK_STATUS_INVALID_BURNER_STATE = 7,
    ACK_STATUS_UNKOWN_ERROR = 8,
    ACK_SATUS_IMAGE_PIECE_TOO_BIG = 9,
    ACK_SATUS_RX_BUFFER_OVERFLOW = 10,
    CK_STATUS_BAD_CRC = 11
};

struct __attribute__((packed)) req_header
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
        return magic == BURNER_MAGIC && (op == INQUIRY_REQ_OP || op == START_PIECE_REQ_OP || op == IMG_DATA_REQ_OP);
    }
};

struct __attribute__((packed)) ack_header
{
    uint16_t magic;
    uint16_t op;
    uint16_t status;
    ack_header(Op opcode, AckStatus status) :
        magic(BURNER_MAGIC),
        op(opcode),
        status(status)
    {
    }
    void swizzle()
    {
        magic = htons(magic);
        op = htons(op);
        status = htons(status);
    }
};

struct __attribute__((packed)) inquiry_req : public req_header
{
    bool validate()
    {
        if (!req_header::validate())
        {
            return false;
        }

        return op == INQUIRY_REQ_OP;
    }
};

struct __attribute__((packed)) inquiry_ack : public ack_header
{
    uint16_t rx_buffer_size;
    uint16_t max_dim;
    inquiry_ack(uint16_t rx_buffer_size, uint16_t max_dim) :
        ack_header(INQUIRY_ACK_OP, ACK_SATUS_SUCCESS),
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

struct __attribute__((packed)) image_piece_header
{
    uint16_t start_x;
    uint16_t start_y;
    uint16_t width;
    uint16_t height;
    void deswizzle()
    {
        start_x = ntohs(start_x);
        start_y = ntohs(start_y);
        width = ntohs(width);
        height = ntohs(height);
    }
    bool validate()
    {
        if (width == 0 || height == 0)
        {
            return false;
        }

        return true;
    }
};

struct __attribute__((packed)) start_piece_req : public req_header
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

struct __attribute__((packed)) start_piece_ack : public ack_header
{
    start_piece_ack() :
        ack_header(START_PIECE_ACK_OP, ACK_SATUS_SUCCESS)
    {
    }
};

struct __attribute__((packed)) image_data_req : public req_header
{
    uint16_t number_of_bytes;   // number of image bytes following this reqeust
    uint16_t image_data_crc;    // CRC of image data beingsent
};

struct __attribute__((packed)) image_data_ack : public ack_header
{
    uint8_t complete;
    image_data_ack(bool complete) :
        ack_header(IMG_DATA_ACK_OP, ACK_SATUS_SUCCESS),
        complete(complete ? 1: 0)
    {
    }
};
