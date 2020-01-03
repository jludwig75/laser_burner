#include "image_receiver.h"
#include "laser_burner.h"
#include "image_router.h"


ImageReceiver image_receiver;
LaserBurner laser_burner;
ImageRouter rotuer(&image_receiver, &laser_burner);

void setup()
{

}

void loop()
{
  rotuer.run();
}
