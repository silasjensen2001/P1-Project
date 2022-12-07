//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported
#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class RoutePlanner{

    //Protected is like private members except that inherited classes can access these members
    protected:

    //Lists
    int stone[4][2] = {{20,24},{20,21},{30,35},{40,45}};
    //int stone[4][2] = {{70, 20},{15, 40},{25,80},{40,70}};
    int stoneList[4][2] = {{70, 20},{15, 40},{25,80},{40,70}};

    //Variables 
    float current_pos[2];      //{x,y} [cm]  
    float i;
    float j;
    float length;

    int l;

    int list_size = 4; // stoneList.size()/size(float);
    float lengthList[4];

    //Underneath here are all methods/functions defined
    public:

        //This is an example function
        void nearest_neighbour(){
            for (int i = 0; i < list_size; i++) {
                for (int j = 0; j < list_size - i; i++) {
                    length = sqrt(pow((stoneList[i][0] - stoneList[j][0]), 2) + pow((stoneList[i][1] - stoneList[j][1]),2));
                    lengthList[i] = length; 
                    l += 1
                }
                Serial.println(lengthList[1]);
                // Iterate the array
            for(int i=0; i < n; i++)
            {
                if(arr[i] < min)
                {
                    //If current value is less than min value
                    // then replace it with min value
                    min = arr[i];
                    index = i;
            }

        }

        void SortXY(){
            float UpdateList[4][2]; //another array lokal for this method/function.
            for(int i = 0; i < 4; i++){ //sets the array UpList=sten.
                UpdateList[i][0]=stone[i][0];
                UpdateList[i][1]=stone[i][1];
            }
            for (size_t i = 0; i < 3; i++){   //makes it check trough the array numStone-1 times.
                for (size_t S = 0; S < 3; S++){  //Checks trough the array comparing 2 parts of the array that is beside each other each time. 
                    if ((stone[S][0] > stone[S+1][0]) || ((stone[S][0] == stone[S+1][0]) && ((stone[S][1] > stone[S+1][1])))){
                        UpdateList[S][0] = stone[S+1][0];
                        UpdateList[S][1] = stone[S+1][1];
                        UpdateList[S+1][0] = stone[S][0];
                        UpdateList[S+1][1] = stone[S][1];          
                        for(int i = 0; i < 4; i++){
                            stone[i][0]=UpdateList[i][0];
                            stone[i][1]=UpdateList[i][1];
                        } 
                    }
                }  
            }   
        }

};   

