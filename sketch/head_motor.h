#pragma once

#include <stdint.h>


class HeadMotor
{
public:
    HeadMotor(uint16_t max_position);
    void goto_position(uint16_t position);
};