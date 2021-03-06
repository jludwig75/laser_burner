#include "burner_image.h"

#include <assert.h>


std::shared_ptr<BurnerImage> BurnerImage::_the_image;

std::shared_ptr<BurnerImage> BurnerImage::create(const std::string & image_file_name, unsigned width, unsigned height)
{
    assert(!_the_image);

    _the_image = std::shared_ptr<BurnerImage>(new BurnerImage(image_file_name, width, height));

    return _the_image;
}

std::shared_ptr<BurnerImage> BurnerImage::get()
{
    assert(_the_image);

    return _the_image;
}

BurnerImage::BurnerImage(const std::string & image_file_name, unsigned width, unsigned height) :
    _image_file_name(image_file_name)
{
    _img.assign(width, height, 1, 1, UCHAR_MAX);
}

unsigned BurnerImage::max_intensity() const
{
    return UCHAR_MAX;
}

unsigned BurnerImage::width() const
{
    return _img.width();
}

unsigned BurnerImage::height() const
{
    return _img.height();
}

void BurnerImage::set_x_y(unsigned x, unsigned y)
{
    assert(x < _img.width());
    assert(y < _img.height());

    _x = x;
    _y = y;
}

void BurnerImage::set_value(unsigned value)
{
    assert(value <= max_intensity());
    _img.atXY(_x, _y) = value;
}

void BurnerImage::save()
{
    _img.save_pnm(_image_file_name.c_str());
}
