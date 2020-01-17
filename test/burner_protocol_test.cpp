#include "catch2/catch.hpp"

#include "serial_interface.h"
#include "burner_protocol_handler.h"
#include "serial_port_test.h"
#include "exec_process.h"
#include <thread>
#include <sstream>
#include <vector>

#define TEST_RX_BUFFER_SIZE 1234
#define TEST_MAX_DIM        1024

class TestProtocolHanlder : public BurnerProtocolHandler::ProtocolHandlerClient
{
public:
    virtual AckStatus handle_inquiry(uint16_t *rx_buffer_size,
                                        uint16_t *max_dim)
    {
        *rx_buffer_size = TEST_RX_BUFFER_SIZE;
        *max_dim = TEST_MAX_DIM;
        return ACK_SATUS_SUCCESS;
    }
    virtual AckStatus handle_start_piece(uint16_t start_x,
                                            uint16_t start_y,
                                            uint16_t width,
                                            uint16_t height)
    {
        return ACK_SATUS_NOT_IMPLEMENTED;
    }
    virtual AckStatus handle_image_data(uint16_t num_bytes,
                                        uint16_t image_data_crc,
                                        SerialInterface *serial,
                                        bool *complete)
    {
        return ACK_SATUS_NOT_IMPLEMENTED;
    }

};

using namespace std;

void run_handler(BurnerProtocolHandler &handler)
{
    handler.on_loop();
}

vector<string> split(const string &str)
{
    stringstream ss(str);

    vector<string> v;
    while(ss)
    {
        string line;
        ss >> line;
        if (line[line.length() - 1] == '\n')
        {
            line.resize(line.length() - 1);
        }
        if (!line.empty())
        {
            v.push_back(line);
        }
    }

    return v;
}

template<typename _T>
bool from_string(const std::string &str, _T &t)
{
    stringstream ss(str);
    ss >> t;
    return ss.good() && ss.eof();
}

template<typename _T>
std::string to_string(_T &t)
{
    stringstream ss;

    ss << t;
    return ss.str();
}

TEST_CASE("test laser burner protocol", "[protocol]") {
    SerialInterface serial_ifc;
    BurnerProtocolHandler protocol_handler(&serial_ifc);
    TestProtocolHanlder test_handler;

    protocol_handler.register_client(&test_handler);
    serial_ifc.begin(9600);

    SECTION("test inquiry request") {
        thread handler_thread(run_handler, ref(protocol_handler));

        string command = string("../exercise_protocol.py -o INQUIRY ") + get_serial_port_name();

        string output = exec(command);

        auto lines = split(output);

        REQUIRE(lines.size() == 3);
        REQUIRE(lines[0] == "SUCCESS");

        REQUIRE(lines[1] == string("RX_BUFFER_SIZE=") + to_string(TEST_RX_BUFFER_SIZE));
        REQUIRE(lines[2] == string("MAX_DIM=") + to_string(TEST_MAX_DIM));

        handler_thread.join();
    }
}