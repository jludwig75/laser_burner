#include "catch2/catch.hpp"

#include "image_receiver.h"
#include "image_router.h"
#include "image_piece.h"

#include "mock_serial_interface.h"
#include "serial_interface.h"

#include <string.h>


using namespace std;

#define TEST_MAX_DIM    22


ImagePiece received_piece;

ImageRouter::ImageRouter(LaserBurner *burner) :
    _burner(burner)
{

}

void ImageRouter::route_image_piece(const ImagePiece *piece)
{
    received_piece = *piece;
}

void generate_random_image_piece(ImagePiece &piece, uint16_t image_dim)
{
    REQUIRE(piece.init(0, 0, image_dim, image_dim));
    uint16_t entries_to_add = image_dim * image_dim;
    uint8_t *buffer = piece.get_data_rx_buffer(entries_to_add);
    REQUIRE(buffer != NULL);
    srand(time(NULL));
    for(uint16_t i = 0; i < entries_to_add; i++)
    {
        buffer[i] = rand() % UINT8_MAX;
    }
    REQUIRE(piece.report_bytes_added_to_rx_buffer(entries_to_add));
}

void add_image_piece_to_rx_buffer(const ImagePiece &piece)
{
    add_bytes_to_receive(vector<uint8_t>(piece.get_data_start_buffer(),
                                            piece.get_data_start_buffer() + piece.total_bytes()));
}

uint16_t crc16(const uint8_t *data_p, uint16_t length);

SCENARIO("test image receiver", "[image_receiver]") {
    ImageRouter router(NULL);
    SerialInterface serial;
    ImageReceiver receiver(&serial, &router, TEST_MAX_DIM);

    uint16_t rx_buffer_size;
    uint16_t max_dim;
    REQUIRE(receiver.handle_inquiry(&rx_buffer_size, &max_dim) == ACK_STATUS_SUCCESS);
    REQUIRE(max_dim == TEST_MAX_DIM);

    // Prove that the piece starts out empty
    REQUIRE(received_piece.total_bytes() == 0);

    GIVEN("a test image smaller than the rx buffer") {
        // Use a size that allows sending the whole piece at once.
        const uint16_t test_image_dim = 7;
        REQUIRE(test_image_dim * test_image_dim < rx_buffer_size);

        ImagePiece test_piece;
        generate_random_image_piece(test_piece, test_image_dim);
        add_image_piece_to_rx_buffer(test_piece);

        WHEN("image data is sent without sending a start request") {
            bool complete;
            AckStatus status = receiver.handle_image_data(test_piece.total_bytes(), crc16(test_piece.get_data_start_buffer(), test_piece.total_bytes()), &serial, &complete);
            THEN("an invalid state error is returned") {
                REQUIRE(status == ACK_STATUS_INVALID_BURNER_STATE);
            }
        }

        // Start the piece
        WHEN("a piece is started that is out of bounds horizontally") {
            AckStatus status = receiver.handle_start_piece(TEST_MAX_DIM / 2, 0, TEST_MAX_DIM, TEST_MAX_DIM);
            THEN("an invalid parameter error is returned") {
                REQUIRE(status == ACK_STATUS_INVALID_PARAMETER);
            }
        }

        WHEN("a piece is started that is out of bounds vertically") {
            AckStatus status = receiver.handle_start_piece(0, TEST_MAX_DIM / 2, TEST_MAX_DIM, TEST_MAX_DIM);
            THEN("an invalid parameter error is returned") {
                REQUIRE(status == ACK_STATUS_INVALID_PARAMETER);
            }
        }

        WHEN("a zero width piece is started") {
            AckStatus status = receiver.handle_start_piece(0, 0, 0, TEST_MAX_DIM);
            THEN("an invalid parameter error is returned") {
                REQUIRE(status == ACK_STATUS_INVALID_PARAMETER);
            }
        }

        WHEN("a zero height piece is started") {
            AckStatus status = receiver.handle_start_piece(0, 0, TEST_MAX_DIM, 0);
            THEN("an invalid parameter error is returned") {
                REQUIRE(status == ACK_STATUS_INVALID_PARAMETER);
            }
        }

        REQUIRE(receiver.handle_start_piece(test_piece.start_x(), test_piece.start_y(), test_piece.width(), test_piece.height()) == ACK_STATUS_SUCCESS);

        WHEN("more image data is sent than the piece started") {
            bool complete;
            AckStatus status = receiver.handle_image_data(rx_buffer_size, 0, &serial, &complete);
            THEN("an RX buffer overflow error is returned") {
                REQUIRE(status == ACK_STATUS_RX_BUFFER_OVERFLOW);
            }
        }

        WHEN("the image is sent in one xfer") {
            bool complete;
            REQUIRE(receiver.handle_image_data(test_piece.total_bytes(), crc16(test_piece.get_data_start_buffer(), test_piece.total_bytes()), &serial, &complete) == ACK_STATUS_SUCCESS);
            REQUIRE(complete);

            THEN("the image is received correctly") {
                REQUIRE(test_piece.total_bytes() == received_piece.total_bytes());
                REQUIRE(memcmp(test_piece.get_data_start_buffer(), received_piece.get_data_start_buffer(), test_piece.total_bytes()) == 0);
            }
        }

        WHEN("too much data is sent in a xfer") {
            bool complete;
            AckStatus status = receiver.handle_image_data(rx_buffer_size, 0, &serial, &complete);

            THEN("a buffer overflow error is returned") {
                REQUIRE(status == ACK_STATUS_RX_BUFFER_OVERFLOW);
            }
        }

        WHEN("the image is sent in two xfers") {
            bool complete;

            uint16_t bytes_to_send = test_piece.total_bytes() / 2;
            REQUIRE(receiver.handle_image_data(bytes_to_send, crc16(test_piece.get_data_start_buffer(), bytes_to_send), &serial, &complete) == ACK_STATUS_SUCCESS);
            REQUIRE(!complete);

            // Make sure no piece has been sent yet.
            REQUIRE(received_piece.total_bytes() == 0);

            uint16_t new_bytes_to_send = test_piece.total_bytes() - bytes_to_send;
            REQUIRE(receiver.handle_image_data(new_bytes_to_send, crc16(test_piece.get_data_start_buffer() + bytes_to_send, new_bytes_to_send), &serial, &complete) == ACK_STATUS_SUCCESS);
            REQUIRE(complete);

            THEN("the image is received correctly") {
                REQUIRE(test_piece.total_bytes() == received_piece.total_bytes());
                REQUIRE(memcmp(test_piece.get_data_start_buffer(), received_piece.get_data_start_buffer(), test_piece.total_bytes()) == 0);
            }
        }
    }

    GIVEN("a test image larger than the rx buffer") {
        const uint16_t test_image_dim = 22;
        REQUIRE(test_image_dim * test_image_dim > rx_buffer_size);
        REQUIRE(test_image_dim * test_image_dim <= MAX_PIECE_BYTES);

        ImagePiece test_piece;
        generate_random_image_piece(test_piece, test_image_dim);
        add_image_piece_to_rx_buffer(test_piece);

        // Start the piece
        REQUIRE(receiver.handle_start_piece(test_piece.start_x(), test_piece.start_y(), test_piece.width(), test_piece.height()) == ACK_STATUS_SUCCESS);

        WHEN("the image is sent in one xfer") {
            bool complete;
            AckStatus status = receiver.handle_image_data(test_piece.total_bytes(), crc16(test_piece.get_data_start_buffer(), test_piece.total_bytes()), &serial, &complete);
            THEN("an RX buffer overflow error is returned") {
                REQUIRE(status == ACK_STATUS_RX_BUFFER_OVERFLOW);
            }
        }

        WHEN("the image is sent in multiple xfers") {
            uint16_t total_bytes_to_send = test_piece.total_bytes();
            uint16_t bytes_sent = 0;
            while (bytes_sent < total_bytes_to_send)
            {
                uint16_t bytes_to_send = min<uint16_t>(rx_buffer_size, total_bytes_to_send - bytes_sent);
                bool complete;
                REQUIRE(receiver.handle_image_data(bytes_to_send, crc16(test_piece.get_data_start_buffer() + bytes_sent, bytes_to_send), &serial, &complete) == ACK_STATUS_SUCCESS);

                bytes_sent += bytes_to_send;
                REQUIRE((bytes_sent <= total_bytes_to_send));

                if (bytes_sent < total_bytes_to_send)
                {
                    // Make sure no piece has been sent yet.
                    REQUIRE(received_piece.total_bytes() == 0);
                    REQUIRE(!complete);
                }
                else
                {
                    REQUIRE(complete);
                }
            }

            THEN("the image is received correctly") {
                REQUIRE(test_piece.total_bytes() == received_piece.total_bytes());
                REQUIRE(memcmp(test_piece.get_data_start_buffer(), received_piece.get_data_start_buffer(), test_piece.total_bytes()) == 0);
            }
        }
    }

    // Reset for the next test
    received_piece = ImagePiece();
    reset_rx_buffer();
}