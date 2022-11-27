import time

stone = [[80,80],[10,80],[10,10],[50,50]]
UpdateList = stone
 

def SortXY():  
    for i in range(3):
        for i1 in range(3-i): 
            if (stone[i1][0] > stone[i1+1][0]) or ((stone[i1][0] == stone[i1+1][0]) and ((stone[i1][1] > stone[i1+1][1]))):
                UpdateList[i1][0] = stone[i1+1][0]
                UpdateList[i1][1] = stone[i1+1][1]
                
                UpdateList[i1+1][0] = stone[i1][0]
                UpdateList[i1+1][1] = stone[i1][1] 


                for i2 in range(4):
                    stone[i2][0] = UpdateList[i2][0]
                    stone[i2][1] = UpdateList[i2][1]                 
                

                time.sleep(500)
                print(stone[i1][0])

        print("   ")

SortXY()