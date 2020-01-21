#!/usr/bin/env python3
import sys
import serial
import argparse
import json
from imageburner.burnerprotocol import InquiryReq, StartPieceReq, ImageDataReq, UnknownReq

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

def send_start_image_piece(serial_port, baud_rate, params):
    serial_con = serial.Serial(serial_port, baud_rate)
    reg_request = StartPieceReq(serial_con, params['x'], params['y'], params['width'], params['height'])
    try:
        ack = reg_request.send()
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
    except Exception as e:
        print('ERROR: %s' % str(e))

def send_image_data(serial_port, baud_rate, params):
    serial_con = serial.Serial(serial_port, baud_rate)
    image_data = params['image_data'].encode()
    reg_request = ImageDataReq(serial_con, len(image_data), params['crc'])
    try:
        ack = reg_request.send(image_data)
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
            print('COMPLETE=%u' % (1 if ack.complete else 0))
    except Exception as e:
        print('ERROR: %s' % str(e))

def send_unknown_op_code(serial_port, baud_rate, opcode):
    # print('sending unknow opcode %u' % opcode)
    serial_con = serial.Serial(serial_port, baud_rate)
    reg_request = UnknownReq(serial_con, opcode)
    try:
        ack = reg_request.send()
        if ack.status != 0:
            print('ERROR: %u' % ack.status)
        else:
            print('SUCCESS')
    except Exception as e:
        print('ERROR: %s' % str(e))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Exercises the burner protocol form the controller side')

    parser.add_argument('serial_port', help='Serial port to connect to')
    parser.add_argument('-b', '--baud-rate', type=int, default=9600, help='baud rate of serial conneciton (default 9600 bps)')
    parser.add_argument('-o', '--opcode', type=str, required=True, help='OPCODE of the request to send (INQUIRY, START, IMGDATA)')
    parser.add_argument('-p', '--params', type=str, default=None, help='JSON parameters needed for START and IMGDATA')

    args = parser.parse_args()

    if args.opcode in ['START', 'IMGDATA'] and not args.params:
        print('ERROR: You must specify parameters as JSON with -p for opcode %s' % args.opcode)
        parser.print_help()
        sys.exit(-1)
    if args.opcode == 'INQUIRY':
        send_inquiry(args.serial_port, args.baud_rate)
    elif args.opcode == 'START':
        send_start_image_piece(args.serial_port, args.baud_rate, json.loads(args.params))
    elif args.opcode == 'IMGDATA':
        send_image_data(args.serial_port, args.baud_rate, json.loads(args.params))
    else:
        try:
            opcode = int(args.opcode)
        except:
            print('ERROR: opcode %s is not supported' % args.opcode)
            sys.exit(-1)
        send_unknown_op_code(args.serial_port, args.baud_rate, opcode)
