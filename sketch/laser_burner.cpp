#include "laser_burner.h"

#include "image_piece.h"
#include "laser_head.h"


LaserBurner::LaserBurner(LaserHead *head) :
    _laser_head(head)
{
}

void LaserBurner::burn_image_piece(const ImagePiece *piece)
{
    for (uint16_t x = 0; x < piece->width(); x++)
    {
        for (uint16_t y = 0; y < piece->height(); y++)
        {
            uint8_t intensity = piece->get_x_y_intensity(x, y);
            if (intensity > 0)  // Only move the head if there is something to burn.
            {
                _laser_head->goto_x_y(piece->start_x() + x, piece->start_y() + y);
                _laser_head->burn(intensity);
            }
        }
    }
}

uint16_t LaserBurner::max_dim() const
{
    return _laser_head->max_dim();
}
