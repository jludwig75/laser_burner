#pragma once

#include <stdint.h>

class HeadMotor;
class Laser;


class LaserHead
{
public:
    LaserHead(HeadMotor *x_motor, HeadMotor *y_motor, Laser *laser);
    void goto_x_y(uint16_t x, uint16_t y);
    void burn(uint8_t intensity);
private:
    HeadMotor *_x_motor;
    HeadMotor *_y_motor;
    Laser *_laser;
};