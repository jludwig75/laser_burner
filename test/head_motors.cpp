#include "head_motors.h"

#include <assert.h>

#include "burner_image.h"


#ifndef min
#define min(x, y)   (x < y ? x : y)
#endif // min


HeadMotors::HeadMotors(uint16_t width, uint16_t height) :
    _width(width),
    _height(height)
{
}

void HeadMotors::goto_x_y(uint16_t x, uint16_t y)
{
    assert(x < _width);
    assert(y < _height);

    BurnerImage::get()->set_x_y(x, y);
}

uint16_t HeadMotors::max_dim() const
{
    // TODO: enforce square for now. May change later
    return min(_width, _height);
}
