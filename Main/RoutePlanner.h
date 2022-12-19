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
    int list_size = 11; 
    int count = 2;
    int counts = 0;

    float length_list[11];
    int route_list[11][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    int current_pos[2];       //{x,y} [cm] 

    int index_list[11];
    int stone_list[11][2] = {{0,0}, {60,20}, {30,90}, {90,20}, {5,65}, {80,95}, {40,90}, {90,5}, {25,80}, {25,70}, {75,5}};
    
         

    //Underneath here are all public methods defined
    public:

        //Fills the stone
        int randomStoneCoords(bool same_coords_allowed = true){
            for(int i = 0; i < list_size; i++){
                bool repeat = true;

                while(repeat){
                    stone_list[i][0] = (rand() % 19 + 1) * 5;
                    stone_list[i][1] = (rand() % 19 + 1) * 5;
                    delay(200); 

                    if(same_coords_allowed){
                        repeat = false;
                    } else {
                        for(int j = 0; j < i; j++){
                            if (stone_list[j][0] == stone_list[i][0] && stone_list[j][0] == stone_list[i][0]) {
                                break;
                            } else if (j == i-1){
                                repeat = false;
                            }
                        }
                    }
                    
                }
                
            }
        }

        // Nearest neighbour function
        void nearest_neighbour(){
            current_pos[0] = 0;
            current_pos[1] = 0;
            float correct_row[11];
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
        
        // Farthest Insertion function
        void farthest_insertion(){
            current_pos[0] = 0;
            current_pos[1] = 0;
            int check_list[11] = {0,0,0,0,0,0,0,0,0,0,0};
            float length_total_list[11] = {0,0,0,0,0,0,0,0,0,0,0};
            float length = sqrt(pow((stone_list[0][0] - current_pos[0]), 2) + pow((stone_list[0][1] - current_pos[1]), 2));
            float longest_length = 0;
            int longest_length_index;
            int shortest_dist_index;
            float route_dist = 0;
            float shortest_dist = 1000000;
            int index = 0;
            // Find and calculate distance to the farthest point from Zumo
            for (size_t i = 0; i < list_size; i++){
                for (size_t j = 0; j < list_size; j++){
                    if ((length <= (sqrt(pow((stone_list[j][0] - current_pos[0]), 2) + pow((stone_list[j][1] - current_pos[1]), 2))))){ 
                        length = (sqrt(pow((stone_list[j][0] - current_pos[0]), 2) + pow((stone_list[j][1] - current_pos[1]), 2)));
                        index = j;
                    }
                }
            }
            // Place the first two points in the route list
            route_list[0][0] = current_pos[0];
            route_list[0][1] = current_pos[1];
            route_list[1][0] = stone_list[index][0];
            route_list[1][1] = stone_list[index][1];
            check_list[0] = 1;
            check_list[index] = 1;
            /*
            Serial.println(route_list[0][0]);
            Serial.println(route_list[0][1]);
            Serial.println(route_list[1][0]);
            Serial.println(route_list[1][1]);
            Serial.println("-----");
            Serial.println(check_list[0]);
            Serial.println(check_list[1]);
            Serial.println(check_list[2]);
            Serial.println(check_list[3]);
            Serial.println(check_list[4]);
            Serial.println("-----");
            */

            while (count < list_size){
                // Calculate the distance to all points outside the list to find the point farthest from any point within the list
                for (int j = 0; j < list_size; j++){
                    length_total_list[j] = 0;
                }
                for (size_t i = 0; i < list_size; i++){
                    for (size_t j = 0; j < list_size; j++){
                        if (check_list[j] == 0 && check_list[i] == 1){
                            length = (sqrt(pow((stone_list[j][0] - stone_list[i][0]), 2) + pow((stone_list[j][1] - stone_list[i][1]), 2)));
                            index = j;
                            length_total_list[j] += length;
                        }
                    }
                }
                /*
                    Serial.println("-----");
                    Serial.println(length_total_list[0]);
                    Serial.println(length_total_list[1]);
                    Serial.println(length_total_list[2]);
                    Serial.println(length_total_list[3]);
                    Serial.println(length_total_list[4]);
                    Serial.println("-----");
                */

                // find the longest length and its index
                for (size_t i = 0; i < list_size; i++){
                    if (longest_length < length_total_list[i]){
                        longest_length = length_total_list[i];
                        longest_length_index = i;
                    }
                }
                check_list[longest_length_index] = 1;

                /*
                Serial.println("-----");
                Serial.println(longest_length);
                Serial.println(longest_length_index);
                Serial.println("-----");
                */

                for (int j = 0; j < list_size; j++){
                    length_total_list[j] = 0;
                }

                /*
                Serial.println("-----");
                Serial.print(route_list[0][0]);
                Serial.println(route_list[0][1]);
                Serial.print(route_list[1][0]);
                Serial.println(route_list[1][1]);
                Serial.print(route_list[2][0]);
                Serial.println(route_list[2][1]);
                Serial.print(route_list[3][0]);
                Serial.println(route_list[3][1]);
                Serial.print(route_list[4][0]);
                Serial.println(route_list[4][1]);
                Serial.println("-----");
                */

                // Place the new point in all possible locations (except index 0) of the array and calculate the distance of the route for each instance.
                for (size_t i = 1; i < list_size; i++){
                    for (size_t t = list_size - 1; t >= i; t--){
                        route_list[t][0] = route_list[t-1][0];
                        route_list[t][1] = route_list[t-1][1];
                    }
                        /*
                        Serial.print(route_list[0][0]);
                        Serial.println(route_list[0][1]);
                        Serial.print(route_list[1][0]);
                        Serial.println(route_list[1][1]);
                        Serial.print(route_list[2][0]);
                        Serial.println(route_list[2][1]);
                        Serial.print(route_list[3][0]);
                        Serial.println(route_list[3][1]);
                        Serial.print(route_list[4][0]);
                        Serial.println(route_list[4][1]);
                        Serial.println("-----");
                        */
                    
                    route_list[i][0] = stone_list[longest_length_index][0];
                    route_list[i][1] = stone_list[longest_length_index][1];

                    for (size_t j = 0; j < list_size; j++){
                        length_total_list[i] += (sqrt(pow((route_list[j+1][0] - route_list[j][0]), 2) + pow((route_list[j+1][1] - route_list[j][1]), 2)));
                    }

                    // Reset and calculate again
                    for (size_t h = i; h < (list_size); h++){
                        route_list[h][0] = route_list[h+1][0];
                        route_list[h][1] = route_list[h+1][1];
                    }
                }

                // Check the length total list and choose the shortest one
                for (size_t i = 1; i < list_size; i++){
                    if (shortest_dist > length_total_list[i]){
                        shortest_dist = length_total_list[i];
                        shortest_dist_index = i;
                    }
                }
                // Place the point in its respective place
                for (size_t j = list_size - 1; j > shortest_dist_index; j--){
                    route_list[j][0] = route_list[j-1][0];
                    route_list[j][1] = route_list[j-1][1];
                }
                route_list[shortest_dist_index][0] = stone_list[longest_length_index][0];
                route_list[shortest_dist_index][1] = stone_list[longest_length_index][1];
                longest_length = 0;
                longest_length_index = 0;
                shortest_dist = 1000000;
                shortest_dist_index = 0;
                count += 1;

                /*
                Serial.println("-----");
                Serial.println("-----");
                Serial.print(route_list[0][0]);
                Serial.println(route_list[0][1]);
                Serial.print(route_list[1][0]);
                Serial.println(route_list[1][1]);
                Serial.print(route_list[2][0]);
                Serial.println(route_list[2][1]);
                Serial.print(route_list[3][0]);
                Serial.println(route_list[3][1]);
                Serial.print(route_list[4][0]);
                Serial.println(route_list[4][1]);
                Serial.println("-----");
                Serial.println(check_list[0]);
                Serial.println(check_list[1]);
                Serial.println(check_list[2]);
                Serial.println(check_list[3]);
                Serial.println(check_list[4]);
                Serial.println("-----");*/
                
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
                            stone_list[i][0] = update_list[i][0];
                            stone_list[i][1] = update_list[i][1];
                        } 
                    }
                }  
            }   
        }
};
