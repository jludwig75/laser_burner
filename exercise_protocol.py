#!/usr/bin/env python3
import serial
import argparse
from imageburner.burnerprotocol import InquiryReq, StartPieceReq, ImageDataReq

def send_inquiry(serial_port, baud_rate):
    serial_con = serial.Serial(serial_port, baud_rate)
    reg_request = InquiryReq(serial_con)
    try:
        ack = reg_request.send()
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
            print('RX_BUFFER_SIZE=%u' % ack.rx_buffer_size)
            print('MAX_DIM=%u' % ack.max_dim)
    except Exception as e:
        print('ERROR: %s' % str(e))

def send_start_image_piece(serial_port, baud_rate, x, y, width, height):
    serial_con = serial.Serial(serial_port, baud_rate)
    reg_request = StartPieceReq(serial_con, x, y, width, height)
    try:
        ack = reg_request.send()
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
    except Exception as e:
        print('ERROR: %s' % str(e))

def send_image_data(serial_port, baud_rate, number_of_bytes, crc):
    serial_con = serial.Serial(serial_port, baud_rate)
    reg_request = ImageDataReq(serial_con, number_of_bytes, crc)
    try:
        ack = reg_request.send()
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
            print('COMPLETE=%s' % str(ack.status))
    except Exception as e:
        print('ERROR: %s' % str(e))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Exercises the burner protocol form the controller side')

    parser.add_argument('serial_port', help='Serial port to connect to')
    parser.add_argument('-b', '--baud-rate', type=int, default=9600, help='baud rate of serial conneciton (default 9600 bps)')
    parser.add_argument('-o', '--opcode', type=str, required=True, help='OPCODE of the request to send (INQUIRY, START, IMGDATA)')

    args = parser.parse_args()

    if args.opcode == 'INQUIRY':
        send_inquiry(args.serial_port, args.baud_rate)
    elif args.opcode == 'IMGDATA':
        send_image_data(args.serial_port, args.baud_rate, 55, 77)
    else:
        print('opcode %s is not supported' % args.opcode)
