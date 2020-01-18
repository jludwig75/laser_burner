#pragma once

#include "serial_interface.h"
#include "image_router.h"
#include "image_receiver.h"
#include "image_building_burner.h"

#include <thread>
#include "CImg.h"


class ImageBuilder
{
public:
    ImageBuilder(uint16_t max_dim);
    void start();
    void stop();
    const cimg_library::CImg<unsigned char> &get_image() const;
private:
    void run();
    bool _run_loop;
    SerialInterface _serial_ifc;
    ImageBuildingBurner _burner;
    ImageRouter _router;
    ImageReceiver _image_receiver;
    std::thread _thread;
};