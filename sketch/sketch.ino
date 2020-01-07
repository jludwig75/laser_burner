#include "image_receiver.h"
#include "laser_burner.h"
#include "image_router.h"
#include "serial_interface.h"

ImageReceiver image_receiver;
LaserBurner laser_burner;
ImageRouter rotuer(&image_receiver, &laser_burner);
SerialInterface serial_ifc;

void setup()
{
  serial_ifc.begin(9600);
  image_receiver.start();
}

void loop()
{
  rotuer.run();
}
