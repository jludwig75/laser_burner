#!/usr/bin/env python3
import argparse
from imageburner import ImageBurner

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Controller for sending images to the laser burner')

    parser.add_argument('serial_device', type=str, help='The name of the serial device for the laser burner')
    parser.add_argument('image_file', type=str, help='Image file to burn')
    parser.add_argument('-o', '--output', type=str, help='Output of image file sent to laser burner for debug')

    args = parser.parse_args()

    burner = ImageBurner(args.serial_device)
    burner.init()
    burner.burn_image(args.image_file, args.output)