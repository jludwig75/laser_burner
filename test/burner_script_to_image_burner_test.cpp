#include "catch2/catch.hpp"

#include "image_builder.h"
#include "exec_process.h"
#include "serial_port_test.h"


#define TEST_MAX_DIM    150

using namespace std;


SCENARIO("test burner script through image burner", "[integration]") {
    ImageBuilder builder(TEST_MAX_DIM);

    builder.start();

    string output = exec(string("../burn_image.py -o testme.pgm ") + get_serial_port_name() + " ../test.bmp");

    // printf("script output: \"%s\"\n", output.c_str());

    printf("\nKilling serial port to unblock test thread. Ignore I/O errors.\n");
    shutdown_serial_port();

    builder.stop();

    builder.get_image().save_pnm("output.pgm");
}