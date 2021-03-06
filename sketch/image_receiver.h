#pragma once

#include "burner_protocol_handler.h"
#include "image_piece.h"

#include <stdint.h>


class ImagePiece;
class SerialInterface;
class ImageRouter;

class ImageReceiver : public BurnerProtocolHandler::ProtocolHandlerClient
{
public:
    ImageReceiver(SerialInterface *serial, ImageRouter *image_router, uint16_t max_dim);
    void on_loop();
    virtual AckStatus handle_inquiry(uint16_t *rx_buffer_size,
                                     uint16_t *max_dim);
    virtual AckStatus handle_start_piece(uint16_t start_x,
                                         uint16_t start_y,
                                         uint16_t width,
                                         uint16_t height);
    virtual AckStatus handle_image_data(uint16_t num_bytes,
                                        uint16_t image_data_crc,
                                        SerialInterface *serial,
                                        bool *complete);
private:
    enum State {Ready, ReadyWaitingForImageData};
    struct PieceState
    {
        PieceState() :
            start_x(UINT16_MAX),
            start_y(UINT16_MAX),
            width(UINT16_MAX),
            height(UINT16_MAX)
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
    } _piece_state;
    SerialInterface *_serial;
    ImageRouter *_image_router;
    BurnerProtocolHandler _protocol_handler;
    uint16_t _max_dim;
    State _state;
    ImagePiece _piece;
};