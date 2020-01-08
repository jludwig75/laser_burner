#include "burner_protocol_handler.h"

#include "serial_interface.h"
#include "burner_protocol.h"

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#define LOG printf
#include <unistd.h>
#define delay   usleep
#include <string.h>


static Op to_ack_op_code(uint16_t opcode)
{
    switch(opcode)
    {
    case INQUIRY_REQ_OP:
        return INQUIRY_REQ_OP;
    case INQUIRY_ACK_OP:
        return INQUIRY_ACK_OP;
    case START_PIECE_REQ_OP:
        return START_PIECE_REQ_OP;
    case START_PIECE_ACK_OP:
        return START_PIECE_ACK_OP;
    case IMG_DATA_REQ_OP:
        return IMG_DATA_REQ_OP;
    case IMG_DATA_ACK_OP:
        return IMG_DATA_ACK_OP;
    case COMPLETE_PIECE_ACK_OP:
        return COMPLETE_PIECE_ACK_OP;
    default:
        return UNKNOWN_OP;
    }
}


BurnerProtocolHandler::BurnerProtocolHandler(SerialInterface *serial) :
    _serial(serial),
    _client(NULL)
{
}

void BurnerProtocolHandler::register_client(ProtocolHandlerClient *client)
{
    _client = client;
}

void BurnerProtocolHandler::complete_image_piece(AckStatus status)
{
}

void BurnerProtocolHandler::on_loop()
{
    req_header header;
    // Wait for a protocol handler
    while(_serial->available() < sizeof(header))
    {
        delay(10);
    }

    size_t bytes_read = _serial->readBytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));
    if (bytes_read != sizeof(header))
    {
        LOG("I/O error reading request header.\n");
        return_failure_ack(UNKNOWN_OP, ACK_STATUS_IO_ERROR);
        return;
    }

    req_header deswizzled_header = header;
    deswizzled_header.deswizzle();
    if (!deswizzled_header.validate())
    {
        LOG("Received invalid request header");
        return_failure_ack(UNKNOWN_OP, ACK_SATUS_INVALID_REQUEST);
        return;
    }

    switch(deswizzled_header.op)
    {
    case INQUIRY_REQ_OP:
        handle_inquiry_request(&header);
        break;
    case START_PIECE_REQ_OP:
        break;
    case IMG_DATA_REQ_OP:
        break;
    default:
        LOG("Received request with invalid opcode %u.\n", header.op);
        return_failure_ack(to_ack_op_code(header.op), ACK_SATUS_INVALID_REQUEST_OP);
        return;
    }
}

void BurnerProtocolHandler::handle_inquiry_request(const req_header *header)
{
    inquiry_req req;

    if (!receive_remaining_req_data(header, &req, sizeof(req)))
    {
        LOG("I/O error receiving remainger of inquiry request.\n");
        return_failure_ack(INQUIRY_REQ_OP, ACK_STATUS_IO_ERROR);
        return; // TODO: Send an ack
    }

    req.deswizzle();
    if (!req.validate())
    {
        LOG("Invalid inquiry request received.\n");
        return_failure_ack(INQUIRY_REQ_OP, ACK_SATUS_INVALID_REQUEST);
        return; // TODO: Send an ack
    }

    uint16_t rx_buffer_size;
    uint16_t max_dim;
    uint16_t status = _client->handle_inquiry(&rx_buffer_size, &max_dim);

    // Send the register ack
    inquiry_ack ack(rx_buffer_size, max_dim);
    ack.swizzle();
    size_t bytes_sent = _serial->writeBytes(reinterpret_cast<const uint8_t *>(&ack), sizeof(ack));
    if (bytes_sent != sizeof(ack))
    {
        LOG("I/O error sending registration ACK. Restarting...\n");
        // Can't really know how to handle this. Don't know if the request was sent or not.
        // TODO: Should we try to send a failure? For now, yes
        return_failure_ack(INQUIRY_REQ_OP, ACK_STATUS_IO_ERROR);
        return;
    }

    // registration complete
    LOG("Registration complete.\n");
}


bool BurnerProtocolHandler::receive_remaining_req_data(const req_header *header, void *req, uint16_t full_req_size)
{
    uint8_t *req_ptr = reinterpret_cast<uint8_t *>(req);
    memset(req_ptr, 0, full_req_size);
    uint16_t remaining_req_bytes = full_req_size - sizeof(*header);
    if (remaining_req_bytes > 0)
    {
        size_t bytes_read = _serial->readBytes(req_ptr + sizeof(*header), remaining_req_bytes);
        if (bytes_read != remaining_req_bytes)
        {
            LOG("I/O error reading remaining %u bytes of %u request.\n", remaining_req_bytes, header->op);
            return false;
        }
    }
    // Don't copy the header until after we got the remaining bytes
    memcpy(req_ptr, header, sizeof(*header));
    return true;
}

void BurnerProtocolHandler::return_failure_ack(Op opcode, AckStatus status)
{
    assert(status != ACK_SATUS_SUCCESS);

    ack_header ack(opcode, status);
    ack.swizzle();
    size_t bytes_sent = _serial->writeBytes(reinterpret_cast<const uint8_t *>(&ack), sizeof(ack));
    if (bytes_sent != sizeof(ack))
    {
        LOG("I/O error sending failure ack for op %u\n", opcode);
    }
}