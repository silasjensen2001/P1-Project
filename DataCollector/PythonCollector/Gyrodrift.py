#Dette program kan bruges til at læse hvad arduinoen printer 
import serial
import csv
import time

ser = serial.Serial('COM8', 9600, timeout=10)
time.sleep(2)

run = True

header = ["Time", "Error"]

file = open('PID.csv', "w", newline="")
writer = csv.writer(file, delimiter=';')
writer.writerow(header)

while run:
    while ser.inWaiting() > 0:
        y = ser.readline().decode("utf-8")
        y = y.rstrip()
        y = y.replace(".", ",")
        y = y.split(";")


        if y[0] == "done":
            run = False
            break
        else:
            writer.writerow(y)

file.close()

