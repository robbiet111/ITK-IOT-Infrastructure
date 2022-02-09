#!/usr/bin/env python3
import serial

if __name__ == "__main__":
        ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
        ser.reset_input_buffer()

        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                line_segments = line.split()
                if (len(line_segments) > 0):
                    if (line_segments[0]=="data:"):
                        print(line_segments[1])
                    else:
                        print(line)
                else:
                    print(line)