#include "laser.h"

#include "burner_image.h"

#include <unistd.h>
#include <assert.h>


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Laser::Laser(uint16_t max_burn_time_ms, uint8_t max_intensity)
{
    assert(max_burn_time_ms > 0);
    assert(max_intensity > 0);
}

void Laser::burn(uint8_t intensity)
{
    assert(intensity <= _max_intensity);

    std::shared_ptr<BurnerImage> burner_image = BurnerImage::get();

    unsigned image_intensity = map(intensity, 0, _max_intensity, 0, burner_image->max_intensity());

    burner_image->set_value(image_intensity);
}
