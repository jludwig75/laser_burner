#include "burner_image.h"

#include <signal.h>

#define DEFAULT_IMAGE_WIDTH 150
#define DEFAULT_IMAGE_HEIGHT 150


extern void setup();
extern void loop();


volatile bool run_loop = true;

void ctrl_c_hnalder(int sig)
{
    run_loop = false;
}

void help()
{
    printf("laser_burner_sim <output_image_name>\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must specify an output file name\n");
        help();
        return -1;
    }
    if (argc > 2)
    {
        printf("Extra parameters, you must specify just and output image file name\n");
        help();
        return -1;
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        help();
        return 0;
    }

    signal(SIGINT, ctrl_c_hnalder); 

    std::shared_ptr<BurnerImage> burner_image = BurnerImage::create(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT);

    printf("Simulation running. Press Ctrl-C to stop and generate output image.\n");

    setup();
    while(run_loop)
    {
        loop();
    }

    printf("Simulation stopped. Saving output image (BMP) as %s\n", argv[1]);
    burner_image->save(argv[1]);

    return 0;
}