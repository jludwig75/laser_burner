#pragma once

#include <stdint.h>


class Laser
{
public:
    Laser(uint16_t max_burn_time_ms, uint8_t max_intensity);
    void burn(uint8_t intensity);
private:
    uint16_t _max_burn_time_ms;
    uint8_t _max_intensity;
};