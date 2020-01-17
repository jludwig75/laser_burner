#include <string>

#include "serial_port.h"
#include "serial_port_test.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */
#include <sys/ioctl.h>


static std::string g_port_name = "";

const std::string & get_serial_port_name()
{
    return g_port_name;
}


HardwareSerial::HardwareSerial() :
    _fd(-1)
{
}

HardwareSerial::~HardwareSerial()
{
    if (_fd != -1)
    {
        close(_fd);
    }
}

int HardwareSerial::begin(unsigned baud_rate)
{
    const char *port_name = "/dev/ptmx";
    _fd = open(port_name,O_RDWR | O_NOCTTY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                                                /* O_RDWR   - Read/Write access to serial port       */
                                                /* O_NOCTTY - No terminal will control the process   */
                                                /* Open in blocking mode,read will wait              */
    if (_fd == -1)
    {
        printf("\n  Error! in Opening %s\n", port_name);
        return -1;
    }

    g_port_name = ptsname(_fd);
    printf("pty device name: %s\n", g_port_name.c_str());
    fflush(stdout);
    grantpt(_fd);
    unlockpt(_fd);

/*---------- Setting the Attributes of the serial port using termios structure --------- */
		
    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr(_fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    /* Setting the Baud rate */
    cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

    /* 8N1 Mode */
    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
    
    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
    
    
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
    
    /* Setting Time outs */
    SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
    SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


    if((tcsetattr(_fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    {
        printf("\n  ERROR ! in Setting attributes");
    }
    else
    {
        printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
    }

    return 0;
}

int HardwareSerial::available() const
{
    size_t bytes_avail;

    if (ioctl(_fd, FIONREAD, &bytes_avail) == -1)
    {
        return -1;
    }

    return bytes_avail;
}

int HardwareSerial::read() const
{
    char data;

    ssize_t len = ::read(_fd, &data, 1);
    if (len != 1) {
        return -1;
    }

    return (int)data;
}

size_t HardwareSerial::read(char* buffer, size_t size)
{
    return readBytes((uint8_t *)buffer, size);
}

size_t HardwareSerial::readBytes(uint8_t* buffer, size_t size)
{
    return ::read(_fd, buffer, size);
}

size_t HardwareSerial::write(uint8_t c)
{
    return ::write(_fd, &c, 1);
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size)
{
    return ::write(_fd, buffer, size);
}


void HardwareSerial::println(const char *line)
{
    const char *eol = "\r\n";
    write((const uint8_t *)line, strlen(line));
    write((const uint8_t *)eol, strlen(eol));
}

void HardwareSerial::println(char c)
{
    char buffer[3] = { 'X', '\r', '\n' };

    buffer[0] = c;

    ssize_t len = write((const uint8_t *)buffer, sizeof(buffer));
}
