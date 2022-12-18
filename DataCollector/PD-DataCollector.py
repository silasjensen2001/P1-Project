#This program can be used to log data from the Zumo
#It is used together with the PID-test method inside ZumoDrive
import serial
import xlsxwriter
import time

#Initialize Serial port
ser = serial.Serial('COM8', 9600, timeout=10)
time.sleep(2)

#Initialize excel workbook document
workbook = xlsxwriter.Workbook('PD-ZumoTest6.xlsx')
header = ["Time", "Error"]
name = "PID"
sheets = {}
sheets[name + str(0)] = workbook.add_worksheet(str(name) + str(0))

run = True
n = 0
i = 0

#Continous loop that logs the data from the Zumo
#Breaks when a "done" command from the Zumo is received
while run:
    #Check if any serial data is available 
    while ser.inWaiting() > 0:
        #Read and clean data
        y = ser.readline().decode("utf-8")
        y = y.rstrip()
        y = y.split(";")

        #If the Zumo has finished one test start logging a new one
        if y[0] == "new":
            #Before creating a new sheet we want to save the kc-, kd- and error values
            sheets[name + str(n)].write(0, 3, f"Kc = {y[1]} and Kd = {y[2]} Ki = {y[3]}") 

            n += 1
            i = 0

            #Add a new sheet to the workbook
            sheets[name + str(n)] = workbook.add_worksheet(str(name) + str(n))
            sheets[name + str(n)].write(0, 0, header[0])
            sheets[name + str(n)].write(0, 1, header[1])

        elif y[0] == "done":
            run = False
            break
        else:
            sheets[name + str(n)].write(i, 0, float(y[0])/1000)     #Time is converted from milliseconds to seconds
            sheets[name + str(n)].write(i, 1, float(y[1]))
            i += 1


#After all tests have been run the data can now be graphed in the workbook
iterations = n
row = 0

#All data sheets are looped through and graphed in the same sheet and in another sheet
for i in range(iterations):
    chart = workbook.add_chart({'type': 'scatter',
                             'subtype': 'straight'})
    
    chart.add_series({
        'name': '',
        'categories': f'={name}{i}!$A$2:$A$500',
        'values':     f'={name}{i}!$B$2:$B$500',
        'data_labels': {'series_name': False}
    })

    
    chart.set_title({ 'name': f'={name}{i}!$D$1',
                        'name_font': {'size': 14}})

    chart.set_x_axis({'name': 'Time [s]', 'major_unit': 0.5}) 

    chart.set_y_axis({'name': 'Angular Error [degrees]'})

    chart.set_legend({'none': True})

    # Insert the chart into the worksheet.
    sheets[name + str(i)].insert_chart('D5', chart)


    #You can not insert the chart two times and it is therefore necessary to repeat the above
    #This is done so all charts can be inserted into one sheet for comparability 
    chart = workbook.add_chart({'type': 'scatter',
                             'subtype': 'straight'})

    chart.add_series({
        'name': '',
        'categories': f'={name}{i}!$A$2:$A$500',
        'values':     f'={name}{i}!$B$2:$B$500',
        'data_labels': {'series_name': False}
    })

    chart.set_title({ 'name': f'={name}{i}!$D$1',
                        'name_font': {'size': 12}})

    chart.set_x_axis({'name': 'Time [s]', 'major_unit': 1}) 

    chart.set_y_axis({ 'major_unit': 0.1,
                        'min': -0.5, 'max': 0.5}) 

    chart.set_legend({'none': True})


    #The chart can now be inserted beside or below the previous chart
    if i%2 == 0: 
        sheets[name + str(n)].insert_chart(f'A{row*11+1}', chart, {'x_scale': 0.59, 'y_scale': 0.75,})  
    else:
        sheets[name + str(n)].insert_chart(f'E{row*11+1}', chart, {'x_offset': 30, 'x_scale': 0.6, 'y_scale': 0.75,})
        row+=1


workbook.close()
