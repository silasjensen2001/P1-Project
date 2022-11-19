#Dette program kan bruges til at læse hvad arduinoen printer 
import serial
import time

ser = serial.Serial('COM4', 9600, timeout=10)


time.sleep(4)


while True:
    while ser.inWaiting() > 0:
        y = ser.readline().decode("utf-8")
        print(y) 
