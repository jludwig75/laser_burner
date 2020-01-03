#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "serial_port.h"


void delay(unsigned ms)
{
    usleep(ms);
}

void loop(HardwareSerial &serial)
{
    char inByte = ' ';
    if (serial.available()) { // only send data back if data has been sent
        char inByte = serial.read(); // read the incoming data
        serial.write(inByte); // send the data back in a new line so that it is not all one long line
    }
    delay(100); // delay for 1/10 of a second
}


int main(int argc, char *argv[])
{
    HardwareSerial port;
    int r = port.begin(9600);
    if (r != 0)
    {
        printf("Error %d opening serial port %s\n", r, argv[1]);
        return -1;
    }

    for(;;)
    {
        loop(port);
    }
    return 0;
}
