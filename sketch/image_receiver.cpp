#include "image_receiver.h"

#include "serial_interface.h"
#include "burner_protocol.h"

#include <assert.h>
#include <stdio.h>
#define LOG printf
#include <unistd.h>
#define delay   usleep


ImageReceiver::ImageReceiver(SerialInterface *serial, uint16_t max_dim) :
    _serial(serial),
    _protocol_handler(serial),
    _max_dim(max_dim),
    _state(Ready)
{
    _protocol_handler.register_client(this);
}

void ImageReceiver::on_loop()
{
    _protocol_handler.on_loop();
}

AckStatus ImageReceiver::handle_inquiry(uint16_t *rx_buffer_size,
                                        uint16_t *max_dim)
{
    if (_state != Ready && _state != ReadyWaitingForImageData)
    {
        // Be lax about the state. There is no reason to not allow
        // a client to send and inquiry after sending a start,
        // but no image data request.
        return ACK_STATUS_INVALID_BURNER_STATE;
    }

    // TODO: get RX buffer size from serial interface
    *rx_buffer_size = 64;
    *max_dim = _max_dim;

    return ACK_SATUS_SUCCESS;
}

AckStatus ImageReceiver::handle_start_piece(uint16_t start_x,
                                            uint16_t start_y,
                                            uint16_t width,
                                            uint16_t height,
                                            uint16_t image_data_crc)
{
    if (_state != Ready && _state != ReadyWaitingForImageData)
    {
        // Be lax about the state. We can let the client
        // send a start piece, abort and send another start piece.
        // Just reset the state parameters.
        return ACK_STATUS_INVALID_BURNER_STATE;
    }

    // Validate parameters
    if (width == 0 || height == 0 || start_x + width > _max_dim || start_y + height > _max_dim)
    {
        return ACK_SATUS_INVALID_PARAMETER;
    }

    _piece_state.start_x = start_x;
    _piece_state.start_y = start_y;
    _piece_state.width = width;
    _piece_state.height = height;
    _piece_state.image_data_crc = image_data_crc;
    _state = ReadyWaitingForImageData;

    return ACK_SATUS_SUCCESS;
}

AckStatus ImageReceiver::handle_image_data(const uint8_t *image_bytes,
                                           uint16_t num_bytes,
                                           SerialInterface *serial)
{
    if (_state != ReadyWaitingForImageData)
    {
        // The client must first send a start piece, before sending image data.
        return ACK_STATUS_INVALID_BURNER_STATE;
    }

    // Has to be set if state is ReadyWaitingForImageData
    assert(_piece_state.is_set());

    // 1. Construct an image piece with the state data

    // 2. Receive the image data from the serial interface

    // 3. Send the image piece to the image router

    // 4. Verify the image data CRC
    
    // 5. Reset the state to Ready
    _state = Ready;

    // 6. Return success when done.
    return ACK_SATUS_NOT_IMPLEMENTED;
}
