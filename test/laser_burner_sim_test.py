#!/usr/bin/env python3
import argparse
import unittest
import sys
import os
import re
import shlex
import subprocess
import time
import signal
from PIL import Image, ImageChops

sim_path = ''
src_dir = ''

class LaserBurnerSimTest(unittest.TestCase):
    TTY_RE = re.compile(r'pty device name: ([\w/]*)')
    def _images_match(self, image1, image2):
        with Image.open(image1) as img1:
            with Image.open(image2) as img2:
                return True if not ImageChops.difference(img1, img2).getbbox() else False

    def _start_laser_burner_sim(self, output_file_name):
        global sim_path

        # run the sim in another process
        command = '%s %s' % (sim_path, output_file_name)
        p = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE)
        for line in p.stdout:
            line = str(line).strip()
            m = self.TTY_RE.search(line)
            if m != None:
                serial_device_name = m.groups()[0]
                break
        return serial_device_name, p

    def _stop_laser_burner_sim(self, p):
        p.stdout.close()
        # Send SIGINT to save the image file
        p.send_signal(signal.SIGINT)
        # Wait for the image to be saved
        time.sleep(1)
        # send SIGKILL to make sure it dies
        p.send_signal(signal.SIGKILL)
        p.wait()

    def setUp(self):
        os.system('rm *.pgm')

    def test_burn_simple_bmp(self):
        global src_dir

        INPUT_IMAGE_NAME = 'test.bmp'
        BURNER_IMAGE_NAME = 'image_to_burn.pgm'
        BURNER_OUTPUT_IMAGE = 'burned_image.pgm'

        # run the sim in another process
        serial_device_name, p = self._start_laser_burner_sim(BURNER_OUTPUT_IMAGE)

        # print('Press enter to continue...')
        # input()

        # run the controller script to burn the image
        os.system('%s %s %s -o %s' % (os.path.join(src_dir, 'burn_image.py'), serial_device_name, os.path.join(src_dir, INPUT_IMAGE_NAME), BURNER_IMAGE_NAME))

        # stop the simulator
        self._stop_laser_burner_sim(p)

        # compare the output images
        self.assertTrue(self._images_match(BURNER_IMAGE_NAME, BURNER_OUTPUT_IMAGE))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test the laser burner simulation')
    parser.add_argument('-b', '--binary', type=str, default='./laser_burner_sim', help='path to the laser_burner_sim library')
    parser.add_argument('-s', '--src', type=str, default='.', help='path to the repo source dir')
    parser.add_argument('unittest_args', nargs='*')
    args = parser.parse_args()

    if args.binary:
        sim_path = args.binary

    if args.src:
        src_dir = args.src

    sys.argv[1:] = args.unittest_args
    unittest.main()
