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
sheets[name + str(0)] = workbook.add_worksheet(str(name) + str(0))

run = True
n = 0
i = 0

while run:
    while ser.inWaiting() > 0:
        y = ser.readline().decode("utf-8")
        y = y.rstrip()
        y = y.split(";")


        if y[0] == "new":
            print("new")
            #Before creating a new sheet we want to save the kc-, kd- and error values
            sheets[name + str(n)].write(0, 3, f"Kc = {y[1]} and Kd = {y[2]}\nTotal Error = {y[3]}")

            n += 1
            i = 0

            sheets[name + str(n)] = workbook.add_worksheet(str(name) + str(n))
            sheets[name + str(n)].write(0, 0, header[0])
            sheets[name + str(n)].write(0, 1, header[1])

        elif y[0] == "done":
            run = False
            break
        else:
            sheets[name + str(n)].write(i, 0, float(y[0])/1000)
            sheets[name + str(n)].write(i, 1, float(y[1])/1000)
            i += 1

iterations = n+1

sheets[name + str(iterations+1)] = workbook.add_worksheet(str(name) + str(iterations+1))
row = 0

for i in range(iterations):
    chart = workbook.add_chart({'type': 'scatter',
                             'subtype': 'straight'})
    #chart = workbook.add_chart({'type': 'line'})

    chart.add_series({
        'name': '',
        'categories': f'={name}{i}!$A$2:$A$500',
        'values':     f'={name}{i}!$B$2:$B$500',
        'data_labels': {'series_name': False}
    })

    # Add a chart title 
    chart.set_title({ 'name': f'={name}{i}!$D$1',
                        'name_font': {'size': 14}})

    # Add x-axis label
    chart.set_x_axis({'name': 'Time [s]', 'major_unit': 0.5}) 

    # Add y-axis label
    chart.set_y_axis({'name': 'Angular Error [degrees]'})

    chart.set_legend({'none': True})

    # Insert the chart into the worksheet.
    sheets[name + str(i)].insert_chart('D5', chart)

    #You can not insert the chart two times and it therefore necessary to creat the chart again
    chart = workbook.add_chart({'type': 'scatter',
                             'subtype': 'straight'})
    #chart = workbook.add_chart({'type': 'line'})

    chart.add_series({
        'name': '',
        'categories': f'={name}{i}!$A$2:$A$500',
        'values':     f'={name}{i}!$B$2:$B$500',
        'data_labels': {'series_name': False}
    })

    # Add a chart title 
    chart.set_title({ 'name': f'={name}{i}!$D$1',
                        'name_font': {'size': 12}})

    # Add x-axis label
    chart.set_x_axis({'name': 'Time [s]', 'major_unit': 1}) 

    # Add y-axis label
    chart.set_y_axis({ 'major_unit': 0.1,
                        'min': -1, 'max': 1}) #'name': 'Angular Error [degrees]'

    chart.set_legend({'none': True})

    if i%2 == 0: 
        sheets[name + str(iterations+1)].insert_chart(f'A{row*11+1}', chart, {'x_scale': 0.59, 'y_scale': 0.75,})  
    else:
        sheets[name + str(iterations+1)].insert_chart(f'E{row*11+1}', chart, {'x_offset': 30, 'x_scale': 0.6, 'y_scale': 0.75,})
        row+=1

workbook.close()
