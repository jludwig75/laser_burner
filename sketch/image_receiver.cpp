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
                                            uint16_t header_crc,
                                            uint16_t image_data_crc)
{
    return ACK_SATUS_NOT_IMPLEMENTED;
}

AckStatus ImageReceiver::handle_image_data(const uint8_t *image_bytes,
                                           uint16_t num_bytes)
{
    return ACK_SATUS_NOT_IMPLEMENTED;
}

// void ImageReceiver::start()
// {
//     inquiry_req req;

//     for(;;)
//     {
//         LOG("Waiting for controller registration");
//         while(_serial->available() < sizeof(req))
//         {
//             // TODO: delay with tinmeout (maybe no timeout).
//             delay(10);
//         }

//         LOG("Reading registration request...\n");
//         // TODO: verify assumption that available bytes will always be read all at once.
//         size_t bytes_read = _serial->readBytes(reinterpret_cast<uint8_t *>(&req), sizeof(req));
//         if (bytes_read != sizeof(req))
//         {
//             LOG("I/O error reading registration. Restarting...\n");
//             continue;  // IO error
//         }

//         LOG("Received registration. Validating...\n");
//         req.deswizzle();
//         if (!req.validate())
//         {
//             // TODO: Log this
//             LOG("Registration not valid. Restarting...\n");
//             continue;
//         }

//         LOG("Registration valid. Sending ACK.\n");
//         // Send the register ack
//         // TODO: get RX buffer size from setial interface
//         inquiry_ack ack(64, _max_dim);
//         ack.swizzle();
//         size_t bytes_sent = _serial->writeBytes(reinterpret_cast<const uint8_t *>(&ack), sizeof(ack));
//         if (bytes_sent != sizeof(ack))
//         {
//             LOG("I/O error sending registration ACK. Restarting...\n");
//             continue;
//         }

//         // registration complete
//         LOG("Registration complete.\n");
//         break;
//     }
// }

// bool ImageReceiver::done() const
// {
//     return true;
// }

// void ImageReceiver::get_next_image_piece(ImagePiece *&piece)
// {
//     start_piece_req req;

//     // Wait for data to arrive
//     while(_serial->available() < sizeof(req))
//     {
//         // TODO: delay with tinmeout.
//         delay(10);
//     }

//     size_t bytes_read = _serial->readBytes(reinterpret_cast<uint8_t *>(&req), sizeof(req));
//     if (bytes_read != sizeof(req))
//     {
//         LOG("I/O error reading start piece req");
//         // TODO: Loop or exit with error??
//         return;
//     }

//     req.deswizzle();
//     if (!req.validate(_max_dim))
//     {
//         LOG("Received invalid start piece req\n");
//         // TODO: Loop or exit with error??
//         return;
//     }
// }

// void ImageReceiver::last_piece_complete()
// {

// }
