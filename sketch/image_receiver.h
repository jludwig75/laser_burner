#pragma once

#include "burner_protocol_handler.h"

#include <stdint.h>


class ImagePiece;
class SerialInterface;

class ImageReceiver : public BurnerProtocolHandler::ProtocolHandlerClient
{
public:
    ImageReceiver(SerialInterface *serial, uint16_t max_dim);
    void on_loop();
    virtual AckStatus handle_inquiry(uint16_t *rx_buffer_size,
                                     uint16_t *max_dim);
    virtual AckStatus handle_start_piece(uint16_t start_x,
                                         uint16_t start_y,
                                         uint16_t width,
                                         uint16_t height,
                                         uint16_t header_crc,
                                         uint16_t image_data_crc);
    virtual AckStatus handle_image_data(const uint8_t *image_bytes,
                                        uint16_t num_bytes);


    // void start();
    // bool done() const;
    // void get_next_image_piece(ImagePiece *&piece);
    // void last_piece_complete();
private:
    SerialInterface *_serial;
    BurnerProtocolHandler _protocol_handler;
    uint16_t _max_dim;
};