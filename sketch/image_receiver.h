#pragma once

class ImagePiece;
class SerialInterface;

class ImageReceiver
{
public:
    void start();
    bool done() const;
    void get_next_image_piece(ImagePiece *&piece);
private:
    SerialInterface *_serial;
};