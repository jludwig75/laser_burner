#pragma once

#include <stdint.h>


class HeadMotors
{
public:
    HeadMotors(uint16_t width, uint16_t height);
    void goto_x_y(uint16_t x, uint16_t y);
    uint16_t max_dim() const;
private:
    uint16_t _width;
    uint16_t _height;
};