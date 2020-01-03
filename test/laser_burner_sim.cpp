#include "burner_image.h"

#define DEFAULT_IMAGE_WIDTH 300
#define DEFAULT_IMAGE_HEIGHT 300


extern void setup();
extern void loop();


int main(int argc, char *argv[])
{
    // Setup the output image and the laser/motors
    BurnerImage::create(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT);

    setup();
    for(;;)
    {
        loop();
    }
    return 0;
}