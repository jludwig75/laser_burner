#pragma once

#include <stdint.h>
#include <stdlib.h>


class HardwareSerial
{
public:
    HardwareSerial();
    ~HardwareSerial();
    int begin(unsigned baud_rate);
    int available() const;
    int read() const;
    size_t read(char* buffer, size_t size);
    size_t readBytes(uint8_t* buffer, size_t size);
    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);
    void println(const char *str);
    void println(char c);
    size_t getRxBufferSize();
private:
    int _fd;
};
