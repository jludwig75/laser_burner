#include "image_builder.h"

using namespace std;
using namespace cimg_library;


ImageBuilder::ImageBuilder(uint16_t max_dim) :
    _run_loop(false),
    _serial_ifc(),
    _burner(max_dim),
    _router(&_burner),
    _image_receiver(&_serial_ifc, &_router, _burner.max_dim())
{
    _serial_ifc.begin(9600);
}

void ImageBuilder::run()
{
    while(_run_loop)
    {
        _image_receiver.on_loop();
    }
}

void ImageBuilder::start()
{
    _run_loop = true;

    // Start thread
    _thread = thread(&ImageBuilder::run, this);
}

void ImageBuilder::stop()
{
    // Tell thread to stop
    _run_loop = false;
    // Join thread
    _thread.join();
}

const cimg_library::CImg<unsigned char> &ImageBuilder::get_image() const
{
    return _burner.get_image();
}
