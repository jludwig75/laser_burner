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
                                         uint16_t image_data_crc);
    virtual AckStatus handle_image_data(const uint8_t *image_bytes,
                                        uint16_t num_bytes,
                                        SerialInterface *serial);
private:
    enum State {Ready, ReadyWaitingForImageData};
    struct PieceState
    {
        PieceState() :
            start_x(UINT16_MAX),
            start_y(UINT16_MAX),
            width(UINT16_MAX),
            height(UINT16_MAX),
            image_data_crc(0)
        {
        }
        bool is_set() const{
            return start_x != UINT16_MAX &&
                    start_y != UINT16_MAX &&
                    width != UINT16_MAX &&
                    height != UINT16_MAX;
        }
        uint16_t start_x;
        uint16_t start_y;
        uint16_t width;
        uint16_t height;
        uint16_t image_data_crc;
    } _piece_state;
    SerialInterface *_serial;
    BurnerProtocolHandler _protocol_handler;
    uint16_t _max_dim;
    State _state;
};