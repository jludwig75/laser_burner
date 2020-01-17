#include "serial_interface.h"

#include <HardwareSerial.h>


int SerialInterface::available()
{
    return Serial.available();
}

size_t SerialInterface::readBytes(uint8_t* buffer, size_t size)
{
    return Serial.readBytes(buffer, size);
}

size_t SerialInterface::writeBytes(const uint8_t* buffer, size_t count)
{
    return Serial.write(buffer, count);
}
