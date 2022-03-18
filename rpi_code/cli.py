#!/usr/bin/env python3

#   Authors:
#     - Dima Maneuski
#     - Jakub Jelinek - 2478625J@student.gla.ac.uk / jakubjjelinek@gmail.com
#     - Robbie Tippen
#   Project: ITk IoT Infrastructure - Aims to develop a platform supporting a CLI and OTA updates on the TTGO Esp32 board.
#
#   Change log:
#   Version: 1.0.0 [18/03/2022]
#   First full release.
#
#   This code is licensed under MIT license (see LICENSE for details)
#

import serial
import argparse
 
if __name__ == "__main__":

    serial_port = '/dev/ttyUSB0'
    parser=argparse.ArgumentParser(description="RPi Serial")

    parser.add_argument('--port',help='Specify port to be monitored')

    args = parser.parse_args()
    if args.port:
        serial_port = args.port

    ser = serial.Serial(serial_port, 115200, timeout=1)
    ser.reset_input_buffer()

    while True:
        command = input(">")
        if command.strip() == "exit":
            break
        else:
            ser.write(command.encode('utf-8'))
