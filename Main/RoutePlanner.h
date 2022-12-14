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
    //float i;
    //float j;
    float length;

    int min = INT_MAX;
    int index;
    int index_list[4];
    //int n = sizeof(length_list);
    //int counts = 0;

    int list_size = 4; // stoneList.size()/size(float);
    float length_list[4];
    float route_list[4][2];

    //Underneath here are all methods/functions defined
    public:

        // Nearest neighbour function
        void nearest_neighbour(){
            current_pos[0] = 0;
            current_pos[1] = 0;
            float correkt_row[4];
            int length = sqrt(pow((stone_list[0][0] - current_pos[0]), 2) + pow((stone_list[0][1] - current_pos[1]), 2));
            int index = 0;
            for (size_t i = 0; i < list_size; i++){
                for (size_t j = 0; j < list_size; j++){
                    if ((index_list[j] == 0) && (j != 0)){
                        if ((length >= (sqrt(pow((stone_list[i][0] - current_pos[0]), 2) + pow((stone_list[i][1] - current_pos[1]), 2)))) || (length == 0)){    //length == 0 since the lenght needs to be updated in the beginning.
                            length = (sqrt(pow((stone_list[i][0] - current_pos[0]), 2) + pow((stone_list[i][1] - current_pos[1]), 2)));
                            index = j;
                        }
                        else{
                            //empty
                        }
                    }
                }
                length = 0;
                index_list[index] = 1;
                correkt_row[i] = index;
                current_pos[0] = stone_list[index][0];
                current_pos[1] = stone_list[index][1];
                route_list[i][0] = stone_list[index][0];
                route_list[i][1] = stone_list[index][1];
            }
            Serial.println(index_list[0]);
            Serial.println(index_list[1]);
            Serial.println(index_list[2]);
            Serial.println(index_list[3]);
            Serial.println(correkt_row[0]);
            Serial.println(correkt_row[1]);
            Serial.println(correkt_row[2]);
            Serial.println(correkt_row[3]);
            Serial.println(route_list[0][0]);
            Serial.println(route_list[0][1]);
            Serial.println("skift");
            Serial.println(route_list[1][0]);
            Serial.println(route_list[1][1]);
            Serial.println(route_list[2][0]);
            Serial.println(route_list[2][1]);
            Serial.println(route_list[3][0]);
            Serial.println(route_list[3][1]);
            delay(5000000);  //sletttt
        }
        
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
};