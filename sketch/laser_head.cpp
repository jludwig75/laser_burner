#include "laser_head.h"

#include "head_motor.h"
#include "laser.h"


LaserHead::LaserHead(HeadMotor *x_motor, HeadMotor *y_motor, Laser *laser) :
    _x_motor(x_motor),
    _y_motor(y_motor),
    _laser(laser)
{
}

void LaserHead::goto_x_y(uint16_t x, uint16_t y)
{
    _x_motor->goto_position(x);
    _y_motor->goto_position(y);
}

void LaserHead::burn(uint8_t intensity)
{
    _laser->burn(intensity);
}
