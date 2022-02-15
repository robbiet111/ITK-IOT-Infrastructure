#!/usr/bin/env python3
import serial
from datetime import datetime
import time
from sharepoint import SharePoint

if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.reset_input_buffer()

    for i in range(3):
        curr_time = datetime.now()
        filename = "data_from_"+curr_time.strftime("%Y%m%d_%H%M%S")+".txt"
        print(filename)
        counter = 10
        with open(filename, "w") as file:
            while counter > 0:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').rstrip()
                    line_segments = line.split()
                    if (len(line_segments) > 0):
                        if (line_segments[0]=="data:"):
                            print(line_segments[1])
                            file.write(datetime.now().strftime("%H%M%S")+": "+line_segments[1]+"\n")
                            time.sleep(1)
                            counter = counter - 1
                        else:
                            print(line)
                    else:
                        print(line)
        folder_name = 'From_serial'

        SharePoint().upload_file(filename, filename, folder_name)
