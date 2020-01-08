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
  |<-- image piece ack   ---|   Success or failure based on data received without timeout and matching CRC
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
    IMG_DATA_ACK_OP = 6,
    COMPLETE_PIECE_ACK_OP = 7
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
    ACK_STATUS_INVALID_BURNER_STATE = 7
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
    ack_header(Op opcode, AckStatus status = ACK_SATUS_SUCCESS) :
        magic(BURNER_MAGIC),
        op(opcode),
        status(status)
    {
    }
    void swizzle()
    {
        magic = htons(magic);
        op = htons(op);
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
        ack_header(INQUIRY_ACK_OP),
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
    bool validate(uint16_t max_dim)
    {
        return true;
    }
};

struct __attribute__((packed)) start_piece_req : public req_header
{
    image_piece_header image_piece;
    bool validate(uint16_t max_dim)
    {
        if (!req_header::validate())
        {
            return false;
        }

        if (op != START_PIECE_REQ_OP)
        {
            return false;
        }

        return image_piece.validate(max_dim);
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
        ack_header(START_PIECE_ACK_OP)
    {
    }
};
