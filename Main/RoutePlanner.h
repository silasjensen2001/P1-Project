//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported

#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class RoutePlanner{

    //Protected is like privat members except that iherited classes can acces these members
    protected:
        float stone[4][2] = {{80,80},{10,80},{10,10},{50,50}};
    //Under here are all methods/functions defined
    public:

        //This is an example function
        void sort_for_freemove(){
            Serial.println("Hello World");
        }
        
        void SortXY(){
            float UpdateList[4][2]; //another array lokal for this method/function.
            for(int i = 0; i < 4; i++){ //sets the array UpList=sten.
                UpdateList[i][0]=stone[i][0];
                UpdateList[i][1]=stone[i][1];
            }
            for (size_t i = 0; i < 3; i++){   //makes it check trough the array numStone-1 times.
                for (size_t S = 0; S < 3; S++){  //Checks trough the array comparing 2 parts, beside each other, of the array each time. 
                    if (stone[S][0] > stone[S+1][0]){
                        UpdateList[S][0] = stone[S+1][0];
                        UpdateList[S][1] = stone[S+1][1];
                        UpdateList[S+1][0] = stone[S][0];
                        UpdateList[S+1][1] = stone[S][1];          
                    }
                    else if ((stone[S][0] = stone[S+1][0]) && ((stone[S][1] > stone[S+1][1]))){
                        UpdateList[S][0] = stone[S+1][0];
                        UpdateList[S][1] = stone[S+1][1];
                        UpdateList[S+1][0] = stone[S][0];
                        UpdateList[S+1][1] = stone[S][1];
                    }
                    for(int i = 0; i < 4; i++){
                            stone[i][0]=UpdateList[i][0];
                            stone[i][1]=UpdateList[i][1];
                    } 
                }  
            }   
        }  
        
};