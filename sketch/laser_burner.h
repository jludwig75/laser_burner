#pragma once

#include <stdint.h>


class ImagePiece;
class LaserHead;


class LaserBurner
{
public:
    LaserBurner(LaserHead *head);

    virtual void burn_image_piece(const ImagePiece *piece);
    virtual uint16_t max_dim() const;
private:
    LaserHead *_laser_head;
};
