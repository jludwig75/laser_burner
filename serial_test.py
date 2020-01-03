#!/usr/bin/env python3
from time import sleep
import serial
import sys

ser = serial.Serial(sys.argv[1], 9600, rtscts=True,dsrdtr=True) # Establish the connection on a specific port
# ser.reset_output_buffer()
# while ser.in_waiting > 0:
#     print(ser.read(ser.in_waiting))
counter = 0 # Below 32 everything in ASCII is gibberish
while True:
    ba = bytearray(1)
    ba[0] = counter
    ser.write(ba) # Convert the decimal number to ASCII then send it to the Arduino
    print(ord(ser.read(1))) # Read the newest output from the Arduino
    sleep(.1) # Delay for one tenth of a second
    counter +=1
    if counter == 256:
        counter = 0