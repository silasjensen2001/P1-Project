//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported
#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include <limits.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class RoutePlanner{

    //Protected is like private members except that inherited classes can access these members
    protected:

    //Lists
    //int stone[4][2] = {{20,24},{20,21},{30,35},{40,45}};
    int stone_list[4][2] = {{70, 10},{15, 20},{25,60},{40,50}};

    //Variables 
    float current_pos[2];      //{x,y} [cm]  
    float i;
    float j;
    float length;

    int min = INT_MAX;
    int index = -1;
    int n = sizeof(length_list);

    int list_size = 4; // stoneList.size()/size(float);
    float length_list[3];
    //float route_list[4][2];

    //Underneath here are all methods/functions defined
    public:

        // Nearest neighbour function
        void nearest_neighbour(){
            Serial.println(n);
            for (int i = 0; i < 4; i++){
                for (int j = 0; j < 4; i++){
                    length = sqrt(pow((stone_list[i][0] - stone_list[j][0]), 2) + pow((stone_list[i][1] - stone_list[j][1]), 2));
            if (length != 0 || length < 0){
                length_list[i] = length;  
            }
            }
            for (int i = 0; i < list_size; i++){
                Serial.println(length_list[i]);
            }
                //Iterate the array to get minimum value in list
            for(int i = 0; i < n; i++){
                Serial.println(length_list[i]);
                if(length_list[i] < min){
                    // If current value is less than min value then replace it with min value
                    min = length_list[i];
                    index = i;
                }
              //route_list[i] = min;
            }
            Serial.println((String)"_____");
            Serial.println((String)"_____");
            Serial.println(index);
           // Serial.println(route_list[0]);
            Serial.println((String)"_____");
            Serial.println(stone_list[1][0]);
    }
};

        void sort_xy(){
            float update_list[4][2]; //another array lokal for this method/function.
            for(int i = 0; i < 4; i++){ //sets the array UpList=sten.
                update_list[i][0]=stone_list[i][0];
                update_list[i][1]=stone_list[i][1];
            }
            for (size_t i = 0; i < 3; i++){   //makes it check trough the array numStone-1 times.
                for (size_t S = 0; S < 3; S++){  //Checks trough the array comparing 2 parts of the array that is beside each other each time. 
                    if ((stone_list[S][0] > stone_list[S+1][0]) || ((stone_list[S][0] == stone_list[S+1][0]) && ((stone_list[S][1] > stone_list[S+1][1])))){
                        update_list[S][0] = stone_list[S+1][0];
                        update_list[S][1] = stone_list[S+1][1];
                        update_list[S+1][0] = stone_list[S][0];
                        update_list[S+1][1] = stone_list[S][1];          
                        for(int i = 0; i < 4; i++){
                            stone_list[i][0]=update_list[i][0];
                            stone_list[i][1]=update_list[i][1];
                        } 
                    }
                }  
            }   
        }

        void sortTSP(){
            Serial.print("Hello world");
        }

};   

