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
from datetime import datetime
import time
from sharepoint import SharePoint
import argparse

if __name__ == "__main__":
    serial_port = '/dev/ttyUSB0'
    freq = 1
    freq_unit = 'd'

    parser=argparse.ArgumentParser(description="RPi Serial")

    parser.add_argument('--port',help='Specify port to be monitored')
    parser.add_argument('--store_freq',help='Specify frequency of storing and uploading files')
    parser.add_argument('--store_freq_unit',choices=['s','m','h','d'],help='Default unit for store_freq is a day [d].\nMinimum unit is [s] - seconds.')
    args = parser.parse_args()

    if args.port:
        serial_port = args.port

    # Storage of data
    if args.store_freq:
        freq=int(args.store_freq)
    if args.store_freq_unit:
        freq_unit = args.store_freq_unit

    if freq_unit == 's':
        freq *= 1
    elif freq_unit == 'm':
        freq *= 60
    elif freq_unit == 'h':
        freq *= 60*60
    elif freq_unit == 'd':
        freq *= 60*60*24

    print(freq, type(freq))
    ser = serial.Serial(serial_port, 115200, timeout=1)
    ser.reset_input_buffer()

    for i in range(3):
        curr_time = datetime.now()
        filename = "MAX31865_"+curr_time.strftime("%Y%m%d_%H%M%S")+".txt"
        print("Data stored in:\n"+filename)
        counter = freq
        with open(filename, "w") as file:
            while counter > 0:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').rstrip()
                    line_segments = line.split()
                    if (len(line_segments) > 0):
                        if (line_segments[0]=="data:"):
                            file.write(line_segments[1]+"\n")
                            time.sleep(1)
                            counter = counter - 1
                        else:
                            print(line)
                    else:
                        print(line)
        folder_name = 'From_serial'
    
        SharePoint().upload_file(filename, filename, folder_name)
