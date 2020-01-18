#include "CImg.h"


using namespace cimg_library;


int main(int argc, char *argv[])
{
    CImg<unsigned char> img;
    img.assign(150, 150, 1, 1, UCHAR_MAX);

    for(size_t x = 0; x < img.width(); x++)
    {
        for(size_t y = 0; y < img.height(); y++)
        {
            img.atXY(x, y) = y;
        }
    }
    
    img.save_pnm("gradient.c.pgm");

    return 0;
}