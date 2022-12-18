//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported

#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The RoutePlanner class contains all methods for route optimization
//that is, sorting methods used on the list of stone coordinates
class RoutePlanner{

    //Protected is like private members except that inherited classes can access these members
    protected:

    float length;

    int index;
    int list_size = 4; 

    float length_list[4];
    float route_list[4][2];
    float current_pos[2];       //{x,y} [cm] 

    int index_list[4];
    int stone_list[4][2] = {{70, 15},{20, 40},{70,60},{50,10}};
    
         

    //Underneath here are all public methods defined
    public:

        //Fills the stone
        int randomStoneCoords(){
            for(int i = 0; i < list_size;i++){
                stone_list[i][0] = (rand() % 16 + 1) * 5;
                stone_list[i][1] = (rand() % 14 + 1) * 5;
                delay(200);
            }
        }

        // Nearest neighbour function
        void nearest_neighbour(){
            current_pos[0] = 0;
            current_pos[1] = 0;
            float correct_row[4];
            int length = sqrt(pow((stone_list[0][0] - current_pos[0]), 2) + pow((stone_list[0][1] - current_pos[1]), 2));
            int index = 0;
            for (size_t i = 0; i < list_size; i++){
                for (size_t j = 0; j < list_size; j++){
                    if (index_list[j] == 0){
                        if ((length >= (sqrt(pow((stone_list[j][0] - current_pos[0]), 2) + pow((stone_list[j][1] - current_pos[1]), 2)))) || (length == 0)){    //length == 0 since the lenght needs to be updated in the beginning.
                            length = (sqrt(pow((stone_list[j][0] - current_pos[0]), 2) + pow((stone_list[j][1] - current_pos[1]), 2)));
                            index = j;
                        }
                        else{
                            //empty
                        }
                    }
                }
                length = 0;
                index_list[index] = 1;
                correct_row[i] = index;
                current_pos[0] = stone_list[index][0];
                current_pos[1] = stone_list[index][1];
                route_list[i][0] = stone_list[index][0];
                route_list[i][1] = stone_list[index][1];
            }      
            for (size_t i = 0; i < list_size; i++){
                stone_list[i][0] = route_list[i][0];
                stone_list[i][1] = route_list[i][1];
            }
        }
        

        //This method sorts the stone coordinates according to the X-axis
        //This is useful when the stone must be picked up using the spray tracks
        void sort_xy(){
            //A temporary list 
            float update_list[4][2];

            //Copy the stone list into update list
            for(int i = 0; i < list_size; i++){ 
                update_list[i][0]=stone_list[i][0];
                update_list[i][1]=stone_list[i][1];
            }

            //Loops through the list and sorts the coordinates by comparing X-coordinates
            //For coordinates with same X-value the coordinates Y-value is compared
            for (size_t i = 0; i < list_size-1; i++){   
                for (size_t S = 0; S < list_size-1; S++){  
                    if ((stone_list[S][0] > stone_list[S+1][0]) || ((stone_list[S][0] == stone_list[S+1][0]) && ((stone_list[S][1] > stone_list[S+1][1])))){
                        update_list[S][0] = stone_list[S+1][0];
                        update_list[S][1] = stone_list[S+1][1];
                        update_list[S+1][0] = stone_list[S][0];
                        update_list[S+1][1] = stone_list[S][1];  

                        for(int i = 0; i < list_size; i++){
                            stone_list[i][0]=update_list[i][0];
                            stone_list[i][1]=update_list[i][1];
                        } 
                    }
                }  
            }   
        }
};