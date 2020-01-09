#include "image_receiver.h"
#include "laser_burner.h"
#include "image_router.h"
#include "serial_interface.h"
#include "laser.h"
#include "head_motors.h"
#include "laser_head.h"

#define MAX_DIM 150
#define MAX_BURN_TIME_MS    10
#define MAX_INTENSITY       255 // UCHAR_MAX

HeadMotors motors(MAX_DIM, MAX_DIM);
Laser laser(MAX_BURN_TIME_MS, MAX_INTENSITY);
LaserHead laser_head(&motors, &laser);
LaserBurner laser_burner(&laser_head);
SerialInterface serial_ifc;
ImageRouter rotuer(&laser_burner);
ImageReceiver image_receiver(&serial_ifc, &rotuer, laser_burner.max_dim());

void setup()
{
    serial_ifc.begin(9600);
}

void loop()
{
    image_receiver.on_loop();
}
