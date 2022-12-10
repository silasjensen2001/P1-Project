#Dette program kan bruges til at læse hvad arduinoen printer 
import serial
import xlsxwriter
import time

ser = serial.Serial('COM8', 9600, timeout=10)
time.sleep(2)

header = ["Time", "Error"]

# Create a workbook and add a worksheet.
workbook = xlsxwriter.Workbook('PD-ZumoTest.xlsx')

#Sheet names
name = "PD"

sheets = {}

run = True
n = 0
i = 0

while run:
    while ser.inWaiting() > 0:
        y = ser.readline().decode("utf-8")
        y = y.rstrip()
        y = y.replace(".", ",")
        y = y.split(";")

        if y[0] == "new":
            #Before creating a new sheet we want to save the kc-, kd- and error values
            sheets[name + str(n)].write(0, 3, f"Kc = {y[1]} and Kd = {2}\nTotal Error = {y[3]}")

            n += 1
            i = 0

            sheets[name + str(n)] = workbook.add_worksheet(str(name) + str(n))
            sheets[name + str(n)].write(0, 0, header[0])
            sheets[name + str(n)].write(0, 1, header[1])
            
        elif y[0] == "done":
            run = False
            break
        else:
            sheets[name + str(n)].write(i, 0, float(y[0]))
            sheets[name + str(n)].write(i, 1, float(y[1]))
            i += 1

workbook.close()
