#include "laser.h"

#include <assert.h>


long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay(long ms)
{
}

Laser::Laser(uint16_t max_burn_time_ms, uint8_t max_intensity)
{
    assert(max_burn_time_ms > 0);
    assert(max_intensity > 0);
}

void Laser::burn(uint8_t intensity)
{
    assert(intensity <= _max_intensity);

    uint16_t burn_time_ms = map(intensity, 0, _max_intensity, 0, _max_burn_time_ms);

    // TODO: Turn laser on

    delay(burn_time_ms);

    // TODO: Turn laser off
}
