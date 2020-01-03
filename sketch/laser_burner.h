#pragma once

#include <stdint.h>


class ImagePiece;
class LaserHead;


class LaserBurner
{
public:
    LaserBurner();

    void burn_image_piece(const ImagePiece *piece);
private:
    LaserHead *_laser_head;
};
