#include "laser_head.h"

#include "head_motors.h"
#include "laser.h"


LaserHead::LaserHead(HeadMotors *motors, Laser *laser) :
    _motors(motors),
    _laser(laser)
{
}

void LaserHead::goto_x_y(uint16_t x, uint16_t y)
{
    _motors->goto_x_y(x, y);
}

void LaserHead::burn(uint8_t intensity)
{
    _laser->burn(intensity);
}

uint16_t LaserHead::max_dim() const
{
    return _motors->max_dim();
}

