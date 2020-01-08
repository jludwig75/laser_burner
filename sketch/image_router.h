#pragma once


class ImageReceiver;
class LaserBurner;


class ImageRouter
{
public:
    ImageRouter(ImageReceiver *receiver, LaserBurner *burner);
    void on_loop();
private:
    ImageReceiver *_receiver;
    LaserBurner *_burner;
};