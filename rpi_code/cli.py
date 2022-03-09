#!/usr/bin/env python3
import serial
 
if __name__ == "__main__":
        ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
        ser.reset_input_buffer()
 
        while True:
            command = input(">")
            if command.strip() == "exit":
                break
            else:
                ser.write(command.encode('utf-8'))
