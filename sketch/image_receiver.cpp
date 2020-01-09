#include "image_receiver.h"

#include "serial_interface.h"
#include "burner_protocol.h"

#include <stdio.h>
#define LOG printf
#include <unistd.h>
#define delay   usleep


ImageReceiver::ImageReceiver(SerialInterface *serial, uint16_t max_dim) :
    _serial(serial),
    _protocol_handler(serial),
    _max_dim(max_dim)
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
    // TODO: get RX buffer size from setial interface
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
    // TODO: Set state to waiting for image data and save off expected data CRC
    return ACK_SATUS_NOT_IMPLEMENTED;
}

AckStatus ImageReceiver::handle_image_data(const uint8_t *image_bytes,
                                           uint16_t num_bytes)
{
    return ACK_SATUS_NOT_IMPLEMENTED;
}

uint16_t ImageReceiver::max_dim() const
{
    return _max_dim;
}
