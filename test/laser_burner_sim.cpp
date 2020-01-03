extern void setup();
extern void loop();



int main(int argc, char *argv[])
{
    // Setup the output image and the laser/motors
    
    setup();
    for(;;)
    {
        loop();
    }
    return 0;
}