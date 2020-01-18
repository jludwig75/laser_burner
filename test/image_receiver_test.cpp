#include "catch2/catch.hpp"

#include "image_receiver.h"
#include "image_router.h"
#include "image_piece.h"


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

SCENARIO("test image receiver", "[image_receiver]") {
    ImageRouter router(NULL);
    ImageReceiver receiver(NULL/*serial*/, &router, TEST_MAX_DIM);

    uint16_t rx_buffer_size;
    uint16_t max_dim;
    REQUIRE(receiver.handle_inquiry(&rx_buffer_size, &max_dim) == ACK_STATUS_SUCCESS);
    REQUIRE(max_dim == TEST_MAX_DIM);

    ImagePiece piece;
    REQUIRE(piece.init(0, 0, TEST_MAX_DIM, TEST_MAX_DIM));
    uint16_t entries_to_add = TEST_MAX_DIM * TEST_MAX_DIM;
    uint8_t *buffer = piece.get_data_rx_buffer(entries_to_add);
    REQUIRE(buffer != NULL);
    srand(time(NULL));
    for(uint16_t i = 0; i < entries_to_add; i++)
    {
        buffer[i] = rand() % UINT8_MAX;
    }
    REQUIRE(piece.report_bytes_added_to_rx_buffer(entries_to_add));

    REQUIRE(receiver.handle_start_piece(piece.start_x(), piece.start_y(), piece.width(), piece.height()) == ACK_STATUS_SUCCESS);

    bool complete;
    REQUIRE(receiver.handle_image_data(entries_to_add, 0, NULL, &complete) == ACK_STATUS_SUCCESS);
    REQUIRE(complete);
}