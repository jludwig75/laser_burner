#include "image_receiver.h"

#include "serial_interface.h"
#include "burner_protocol.h"
#include "image_router.h"
#include "image_piece.h"

#include <assert.h>
#include <stdio.h>
#define LOG printf
#include <unistd.h>
#define delay   usleep


#define POLY 0x8408

static uint16_t crc16(const uint8_t *data_p, uint16_t length)
{
    uint16_t i;
    uint32_t data;
    uint32_t crc = 0xffff;

    if (length == 0)
    {    
        return static_cast<uint16_t>(~crc);
    }

    do
    {
        for (i = 0, data = static_cast<int32_t>(0xff & *data_p++);
             i < 8;
             i++, data >>= 1)
        {
            if (((crc & 0x0001) ^ (data & 0x0001)) != 0)
            {
                crc = (crc >> 1) ^ POLY;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    while (--length > 0);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);

    return static_cast<uint16_t>(crc);
}


ImageReceiver::ImageReceiver(SerialInterface *serial, ImageRouter *image_router, uint16_t max_dim) :
    _serial(serial),
    _image_router(image_router),
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

    *rx_buffer_size = _serial->getRxBufferSize();
    *max_dim = _max_dim;

    return ACK_STATUS_SUCCESS;
}

AckStatus ImageReceiver::handle_start_piece(uint16_t start_x,
                                            uint16_t start_y,
                                            uint16_t width,
                                            uint16_t height)
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
        return ACK_STATUS_INVALID_PARAMETER;
    }

    _piece_state.start_x = start_x;
    _piece_state.start_y = start_y;
    _piece_state.width = width;
    _piece_state.height = height;
    _state = ReadyWaitingForImageData;

    // 1. Construct an image piece with the state data
    if (!_piece.init(_piece_state.start_x, _piece_state.start_y, _piece_state.width, _piece_state.height))
    {
        return ACK_STATUS_IMAGE_PIECE_TOO_BIG;
    }

    return ACK_STATUS_SUCCESS;
}

AckStatus ImageReceiver::handle_image_data(uint16_t num_bytes,
                                           uint16_t image_data_crc,
                                           SerialInterface *serial,
                                           bool *complete)
{
    if (_state != ReadyWaitingForImageData)
    {
        // The client must first send a start piece, before sending image data.
        return ACK_STATUS_INVALID_BURNER_STATE;
    }

    if (num_bytes > _serial->getRxBufferSize())
    {
        return ACK_STATUS_RX_BUFFER_OVERFLOW;
    }

    // Has to be set if state is ReadyWaitingForImageData
    assert(_piece_state.is_set());

    // 2. Validate the image data size
    uint16_t max_rx_bytes = _piece.rx_buffer_bytes_remaining();
    if (num_bytes > max_rx_bytes)
    {
        return ACK_STATUS_RX_BUFFER_OVERFLOW;
    }

    // 3. Receive the image data from the serial interface
    uint8_t *rx_buffer = _piece.get_data_rx_buffer(num_bytes);
    if (!rx_buffer)
    {
        return ACK_STATUS_RX_BUFFER_OVERFLOW;
    }

    size_t total_bytes_received = 0;
    while (total_bytes_received < num_bytes)    // TODO: Add timeout
    {
        size_t bytes_received = serial->readBytes(rx_buffer, num_bytes);
        if (bytes_received == 0)
        {
            return ACK_STATUS_IO_ERROR;
        }
        assert(bytes_received <= num_bytes);

        total_bytes_received += bytes_received;
    }

    _piece.report_bytes_added_to_rx_buffer(num_bytes);

/*  Test CRC after testing the transfer code.
    // 4. Verify the image data CRC
    uint16_t crc = crc16(rx_buffer, num_bytes);
    if (crc != image_data_crc)
    {
        return ACK_STATUS_BAD_CRC;
    }
*/    
    // 5. Send the image piece to the image router
    if (_piece.is_complete())
    {
        _image_router->route_image_piece(&_piece);
        // 5. Reset the state to Ready
        _state = Ready;

        // Tell the caller the image piece is complete
        *complete = true;
    }
    else
    {
        *complete = false;
    }
    

    // 6. Return success when done.
    return ACK_STATUS_SUCCESS;
}
