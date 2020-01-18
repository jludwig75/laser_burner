#include "mock_serial_interface.h"
#include "serial_interface.h"

#include <string.h>


using namespace std;

vector<uint8_t> buffer_to_receive;
vector<uint8_t> buffer_sent;

void add_bytes_to_receive(const std::vector<uint8_t> &bytes)
{
    buffer_to_receive.insert(buffer_to_receive.end(), bytes.begin(), bytes.end());
}

void reset_rx_buffer()
{
    buffer_to_receive.resize(0);
}


void SerialInterface::begin(unsigned long baud)
{
    
}

int SerialInterface::available()
{
    return 0;
}

size_t SerialInterface::readBytes(uint8_t* buffer, size_t size)
{
    size_t bytes_to_read = min(size, buffer_to_receive.size());
    if (bytes_to_read == 0)
    {
        return 0;
    }

    memcpy(buffer, &buffer_to_receive[0], bytes_to_read);
    buffer_to_receive.erase(buffer_to_receive.begin(), buffer_to_receive.begin() + bytes_to_read);
    return bytes_to_read;
}

size_t SerialInterface::writeBytes(const uint8_t* buffer, size_t count)
{
    return 0;
}

size_t SerialInterface::getRxBufferSize()
{
    return 64;  // Match Arduino Uno R3
}
