#!/usr/bin/env python3
import serial
from datetime import datetime
from sharepoint import SharePoint

if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.reset_input_buffer()

    time = datetime.now()
    filename = "data_from_"+time.strftime("%Y%m%d")+".txt"
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
                        file.write(line_segments[1]+"\n")
                        counter = counter - 1
                    else:
                        print(line)
                else:
                    print(line)
    folder_name = 'From_serial'

    SharePoint().upload_file(filename, filename, folder_name)