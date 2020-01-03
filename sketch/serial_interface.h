#pragma once

#include <stdint.h>
#include <stdlib.h>


class SerialInterface
{
public:
    void begin(unsigned long baud);
    int available();
    size_t readBytes(uint8_t* buffer, size_t size);
    size_t writeBytes(const uint8_t* buffer, size_t count);
};