#include <iostream>
#include <cmath>
//#include <arduino.h>

using namespace std;
float length;

int idx;
int list_size = 5; 
int count = 2;
int counts = 0;

float length_list[5];
int route_list[6][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}, {0,0}};
int current_pos[2];       //{x,y} [cm] 

int index_list[5];
int stone_list[5][2] = {{0,0}, {70,15},{20,40},{70,60},{50,10}};

void printt(){
    for (int j = 0; j < list_size; j++){
        cout << route_list[j][0];
        cout << route_list[j][1];
        cout << "\n";    
    }
    
}

void farthest_insertion(){
    current_pos[0] = 0;
    current_pos[1] = 0;
    int check_list[5] = {0,0,0,0,0};
    float length_total_list[5] = {0,0,0,0,0};
    float length = sqrt(pow((stone_list[0][0] - current_pos[0]), 2) + pow((stone_list[0][1] - current_pos[1]), 2));
    float longest_length = 0;
    int longest_length_index;
    int shortest_dist_index;
    float route_dist = 0;
    float shortest_dist = 1000000;
    int index = 0;
    
    // Find and calculate distance to the farthest point from Zumo
    for (int i = 0; i < list_size; i++){
        for (int j = 0; j < list_size; j++){
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

    printt();
    cout << "\n\n";
    //Hertil ok

    while (count < list_size){
        for (int j = 0; j < list_size; j++){
            length_total_list[j] = 0;            
        }

        // Calculate the distance to all points outside the list to find the point farthest from any point within the list
        for (int i = 0; i < list_size; i++){
            for (int j = 0; j < list_size; j++){
                if (check_list[j] == 0 && check_list[i] == 1){
                    length = (sqrt(pow((stone_list[j][0] - stone_list[i][0]), 2) + pow((stone_list[j][1] - stone_list[i][1]), 2)));
                    length_total_list[j] += length;
                }
            }
        }


        cout << "\n"; 
        cout << "\n"; 
        // find the longest length and its index
        for (int i = 0; i < list_size; i++){
            if (longest_length < length_total_list[i]){
                longest_length = length_total_list[i];
                longest_length_index = i;
            }
        }
        check_list[longest_length_index] = 1;

        cout << longest_length_index;
        cout << "\n";
        cout << stone_list[longest_length_index][0];
        cout << stone_list[longest_length_index][1];
        cout << "\n\n";

        for (int j = 0; j < list_size; j++){
            length_total_list[j] = 0;            
        }

        //printt();
        // Place the new point in all possible locations (except index 0) of the array and calculate the distance of the route for each instance.
        for (int i = 1; i < list_size; i++){
            for (int t = list_size - 1; t > i; t--){//>=
                route_list[t][0] = route_list[t-1][0];
                route_list[t][1] = route_list[t-1][1];
            }
            cout << "\n";
            
            route_list[i][0] = stone_list[longest_length_index][0];
            route_list[i][1] = stone_list[longest_length_index][1];
            printt();
            cout << "\n";
            
            for (int j = 0; j < list_size; j++){
                length_total_list[i] += (sqrt(pow((route_list[j+1][0] - route_list[j][0]), 2) + pow((route_list[j+1][1] - route_list[j][1]), 2)));
            }
            
            cout << "\n";

            // Reset and calculate again
            for (int h = i; h < list_size; h++){
                route_list[h][0] = route_list[h+1][0];
                route_list[h][1] = route_list[h+1][1];
            }      
        }
        
        //printt();

       
        // Check the length total list and choose the shortest one
        for (int n = 1; n < list_size; n++){
            if (shortest_dist > length_total_list[n]){
                shortest_dist = length_total_list[n];
                shortest_dist_index = n;
                cout << "shortest";
                cout << shortest_dist_index;
                cout << "\n";
            }
            
        }


        //printt();
        // Place the point in its respective place
        for (int j = list_size - 1; j > shortest_dist_index; j--){
            route_list[j][0] = route_list[j-1][0];
            route_list[j][1] = route_list[j-1][1];
        }
        


        route_list[shortest_dist_index][0] = stone_list[longest_length_index][0];
        route_list[shortest_dist_index][1] = stone_list[longest_length_index][1];
        //printt();
        //cout << "\n";

        longest_length = 0;
        longest_length_index = 0;
        shortest_dist = 1000000;
        shortest_dist_index = 0;
        count += 1;

    }

    /*
    for (int j = 0; j < list_size; j++){
        cout << route_list[j][0];
        cout << route_list[j][1];
        cout << "\n";    
    }*/


}

int main()
{   
    farthest_insertion();
    printt();
  
}

