#include "image_router.h"

#include "image_receiver.h"
#include "laser_burner.h"
#include "image_piece.h"


ImageRouter::ImageRouter(ImageReceiver *receiver, LaserBurner *burner) :
    _receiver(receiver),
    _burner(burner)
{

}

void ImageRouter::run()
{
    while (!_receiver->done())
    {
        ImagePiece *piece;
        _receiver->get_next_image_piece(piece);
        _burner->burn_image_piece(piece);
    }
}
