#!/usr/bin/env python3
import serial
from datetime import datetime
import time
from sharepoint import SharePoint
import argparse

if __name__ == "__main__":
    upload_port = '/dev/ttyUSB0'
    parser=argparse.ArgumentParser(description="RPi Serial")
    
    parser.add_argument('--port',nargs='?', help='Specify port to be monitored')
    args=parser.parse_args()
    print(args)
       
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.reset_input_buffer()

    for i in range(3):
        curr_time = datetime.now()
        filename = "MAX31865_"+curr_time.strftime("%Y%m%d_%H%M%S")+".txt"
        print("Data stored in:\n"+filename)
        counter = 10
        with open(filename, "w") as file:
            while counter > 0:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').rstrip()
                    line_segments = line.split()
                    if (len(line_segments) > 0):
                        if (line_segments[0]=="data:"):
                            file.write(datetime.now().strftime("%H%M%S")+": "+line_segments[1]+"\n")
                            time.sleep(1)
                            counter = counter - 1
                        else:
                            print(line)
                    else:
                        print(line)
        folder_name = 'From_serial'
    
        SharePoint().upload_file(filename, filename, folder_name)