#pragma once

#include <stdint.h>

class SerialInterface;

// class req_header;
// enum Op;
// enum AckStatus;
#include "burner_protocol.h" // TODO: Hide this in cpp

class BurnerProtocolHandler
{
public:
    BurnerProtocolHandler(SerialInterface *serial);
    class ProtocolHandlerClient
    {
    public:
        virtual ~ProtocolHandlerClient()
        {

        }
        virtual AckStatus handle_inquiry(uint16_t *rx_buffer_size,
                                         uint16_t *max_dim) = 0;
        virtual AckStatus handle_start_piece(uint16_t start_x,
                                             uint16_t start_y,
                                             uint16_t width,
                                             uint16_t height,
                                             uint16_t image_data_crc) = 0;
        virtual AckStatus handle_image_data(uint16_t num_bytes,
                                            uint16_t image_data_crc,
                                            SerialInterface *serial,
                                            bool *complete) = 0;
    };
    void register_client(ProtocolHandlerClient *client);
    void complete_image_piece(AckStatus status);
    void on_loop();
private:
    bool receive_remaining_req_data(const req_header *header, void *req, uint16_t full_req_size);

    void handle_inquiry_request(const req_header *header);
    void handle_start_piece_request(const req_header *header);
    void handle_image_data_request(const req_header *header);
    void return_failure_ack(Op opcode, AckStatus status);

    SerialInterface *_serial;
    ProtocolHandlerClient *_client;
};