#pragma once

#include <stdint.h>

class HeadMotors;
class Laser;


class LaserHead
{
public:
    LaserHead(HeadMotors *motors, Laser *laser);
    void goto_x_y(uint16_t x, uint16_t y);
    void burn(uint8_t intensity);
private:
    HeadMotors *_motors;
    Laser *_laser;
};