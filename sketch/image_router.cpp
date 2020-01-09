#include "image_router.h"

#include "image_receiver.h"
#include "laser_burner.h"
#include "image_piece.h"


ImageRouter::ImageRouter(LaserBurner *burner) :
    _burner(burner)
{
}

void ImageRouter::route_image_piece(const ImagePiece *piece)
{
}
