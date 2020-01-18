#include "serial_interface.h"

#include "serial_port.h"

#include <assert.h>


HardwareSerial serial;

void SerialInterface::begin(unsigned long baud)
{
    int ret = serial.begin(baud);
    assert(ret == 0);   // TODO: handle better
}

int SerialInterface::available()
{
    return serial.available();
}

size_t SerialInterface::readBytes(uint8_t* buffer, size_t size)
{
    return serial.readBytes(buffer, size);
}

size_t SerialInterface::writeBytes(const uint8_t* buffer, size_t count)
{
    return serial.write(buffer, count);
}

size_t SerialInterface::getRxBufferSize()
{
    return serial.getRxBufferSize();
}

