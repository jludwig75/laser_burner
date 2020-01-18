#include <thread>
#include "string_utils.h"

#include "catch2/catch.hpp"

#include "serial_interface.h"
#include "burner_protocol_handler.h"
#include "serial_port_test.h"
#include "exec_process.h"


using namespace std;


#define TEST_RX_BUFFER_SIZE 1234
#define TEST_MAX_DIM        1024


class TestProtocolHanlder : public BurnerProtocolHandler::ProtocolHandlerClient
{
public:
    class StartPieceData
    {
    public:
        StartPieceData() :
            _x(UINT16_MAX),
            _y(UINT16_MAX),
            _width(UINT16_MAX),
            _height(UINT16_MAX)
        {
        }
        void set(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
        {
            _x = x;
            _y = y;
            _width = width;
            _height = height;
        }
        uint16_t x() const
        {
            return _x;
        }
        uint16_t y() const
        {
            return _y;
        }
        uint16_t width() const
        {
            return _width;
        }
        uint16_t height() const
        {
            return _height;
        }
    private:
        uint16_t _x;
        uint16_t _y;
        uint16_t _width;
        uint16_t _height;
    };
    class ImageDataData
    {
    public:
        ImageDataData() :
            _number_of_bytes(UINT16_MAX),
            _image_data_crc(UINT16_MAX)
        {

        }
        void set(uint16_t number_of_bytes, uint16_t image_data_crc)
        {
            _number_of_bytes = number_of_bytes;
            _image_data_crc = image_data_crc;
        }
        uint16_t number_of_bytes() const
        {
            return _number_of_bytes;
        }
        uint16_t image_data_crc() const
        {
            return _image_data_crc;
        }
    private:
        uint16_t _number_of_bytes;
        uint16_t _image_data_crc;
    };
    virtual AckStatus handle_inquiry(uint16_t *rx_buffer_size,
                                        uint16_t *max_dim)
    {
        *rx_buffer_size = TEST_RX_BUFFER_SIZE;
        *max_dim = TEST_MAX_DIM;
        return ACK_STATUS_SUCCESS;
    }
    virtual AckStatus handle_start_piece(uint16_t start_x,
                                            uint16_t start_y,
                                            uint16_t width,
                                            uint16_t height)
    {
        if (width == 0 || height == 0 || start_x + width > TEST_MAX_DIM || start_y + height > TEST_MAX_DIM)
        {
            return ACK_STATUS_INVALID_PARAMETER;
        }
        if (width * height > TEST_RX_BUFFER_SIZE)
        {
            return ACK_STATUS_IMAGE_PIECE_TOO_BIG;
        }
        _start_piece_data.set(start_x, start_y, width, height);
        return ACK_STATUS_SUCCESS;
    }
    virtual AckStatus handle_image_data(uint16_t num_bytes,
                                        uint16_t image_data_crc,
                                        SerialInterface *serial,
                                        bool *complete)
    {
        _image_data_data.set(num_bytes, image_data_crc);
        return ACK_STATUS_SUCCESS;
    }
    const StartPieceData &start_piece_data() const
    {
        return _start_piece_data;
    }
    const ImageDataData & image_data_data() const
    {
        return _image_data_data;
    }
private:
    StartPieceData _start_piece_data;
    ImageDataData _image_data_data;
};


void run_handler(BurnerProtocolHandler &handler)
{
    handler.on_loop();
}


TEST_CASE("test laser burner protocol", "[protocol]") {
    SerialInterface serial_ifc;
    BurnerProtocolHandler protocol_handler(&serial_ifc);
    TestProtocolHanlder test_handler;

    protocol_handler.register_client(&test_handler);
    serial_ifc.begin(9600);

    thread handler_thread(run_handler, ref(protocol_handler));

    SECTION("test inquiry request") {
        string command = string("../exercise_protocol.py -o INQUIRY ") + get_serial_port_name();

        string output = exec(command);

        auto lines = split(output);
        REQUIRE(lines.size() == 3);
        REQUIRE(lines[0] == "SUCCESS");
        REQUIRE(lines[1] == string("RX_BUFFER_SIZE=") + to_string(TEST_RX_BUFFER_SIZE));
        REQUIRE(lines[2] == string("MAX_DIM=") + to_string(TEST_MAX_DIM));
    }

    SECTION("test start piece request") {
        // prove that it was not already set to the values expected after running the test
        REQUIRE(test_handler.start_piece_data().x() != 45);
        REQUIRE(test_handler.start_piece_data().y() != 47);
        REQUIRE(test_handler.start_piece_data().width() != 98);
        REQUIRE(test_handler.start_piece_data().height() != 111);

        string command = string("../exercise_protocol.py -o START -p '{\"x\": 45, \"y\": 47, \"width\": 98, \"height\": 11}' ") + get_serial_port_name();

        string output = exec(command);

        auto lines = split(output);
        REQUIRE(lines.size() >= 1);
        REQUIRE(lines[0] == "SUCCESS");

        // Make sure the data was sent correctly
        REQUIRE(test_handler.start_piece_data().x() == 45);
        REQUIRE(test_handler.start_piece_data().y() == 47);
        REQUIRE(test_handler.start_piece_data().width() == 98);
        REQUIRE(test_handler.start_piece_data().height() == 11);
    }

    SECTION("test image data request") {
        // prove that it was not already set to the values expected after running the test
        REQUIRE(test_handler.image_data_data().number_of_bytes() != 45);
        REQUIRE(test_handler.image_data_data().image_data_crc() != 47);

        string command = string("../exercise_protocol.py -o IMGDATA -p '{\"number_of_bytes\": 45, \"crc\": 47}' ") + get_serial_port_name();

        string output = exec(command);

        auto lines = split(output);
        REQUIRE(lines.size() >= 1);
        REQUIRE(lines[0] == "SUCCESS");

        // Make sure the data was sent correctly
        REQUIRE(test_handler.image_data_data().number_of_bytes() == 45);
        REQUIRE(test_handler.image_data_data().image_data_crc() == 47);
    }

    handler_thread.join();
}