#include "image_building_burner.h"

#include "image_piece.h"

#include <stdlib.h>
#include <assert.h>

using namespace cimg_library;


ImageBuildingBurner::ImageBuildingBurner(uint16_t max_dim) :
    LaserBurner(NULL)
{
    _img.assign(max_dim, max_dim, 1, 1, UCHAR_MAX);
}

void ImageBuildingBurner::burn_image_piece(const ImagePiece *piece)
{
    // printf("Buidling %ux%u piece at (%u, %u)\n", piece->width(), piece->height(), piece->start_x(), piece->start_y());
    for (uint16_t x = 0; x < piece->width(); x++)
    {
        for (uint16_t y = 0; y < piece->height(); y++)
        {
            uint8_t intensity = piece->get_x_y_intensity(x, y);
            if (intensity > 0)  // Only move the head if there is something to burn.
            {
                _img.atXY(piece->start_x() + x, piece->start_y() + y) = UINT8_MAX - intensity;
            }
        }
    }
}

uint16_t ImageBuildingBurner::max_dim() const
{
    return _img.width();
}

const cimg_library::CImg<unsigned char> &ImageBuildingBurner::get_image() const
{
    return _img;
}


// Stubs for base class
LaserBurner::LaserBurner(LaserHead *head) :
    _laser_head(head)
{
}

void LaserBurner::burn_image_piece(const ImagePiece *piece)
{
    assert(false && "don't call this");
}

uint16_t LaserBurner::max_dim() const
{
    assert(false && "don't call this");
    return 0;
}
