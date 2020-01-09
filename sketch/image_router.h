#pragma once

class LaserBurner;
class ImagePiece;


class ImageRouter
{
public:
    ImageRouter(LaserBurner *burner);
    void route_image_piece(const ImagePiece *piece);
private:
    LaserBurner *_burner;
};