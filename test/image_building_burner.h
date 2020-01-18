#pragma once

#include "laser_burner.h"

#include "CImg.h"


class ImageBuildingBurner : public LaserBurner
{
public:
    ImageBuildingBurner(uint16_t max_dim);
    virtual void burn_image_piece(const ImagePiece *piece);
    virtual uint16_t max_dim() const;
    const cimg_library::CImg<unsigned char> &get_image() const;
private:
    cimg_library::CImg<unsigned char> _img;
};