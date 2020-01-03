#pragma once

#include <memory>

#include "CImg.h"
using namespace cimg_library;


class BurnerImage
{
public:
    static std::shared_ptr<BurnerImage> create(unsigned width, unsigned height);
    static std::shared_ptr<BurnerImage> get();
    unsigned max_intensity() const;
    unsigned width() const;
    unsigned height() const;
    void set_x_y(unsigned x, unsigned y);
    void set_value(unsigned value);
    void save(const char *file_name);
protected:
    BurnerImage(unsigned width, unsigned height);
private:
    CImg<unsigned char> _img;
    unsigned _x;
    unsigned _y;

    static std::shared_ptr<BurnerImage> _the_image;
};

