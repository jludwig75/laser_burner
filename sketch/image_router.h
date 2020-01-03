#pragma once


class ImageReceiver;
class LaserBurner;


class ImageRouter
{
public:
    ImageRouter(ImageReceiver *receiver, LaserBurner *burner);
    void run();
private:
    ImageReceiver *_receiver;
    LaserBurner *_burner;
};