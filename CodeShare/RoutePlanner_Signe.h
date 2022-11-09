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
        int stoneCount = rand() % 50 + 1;
        int maxStones = 50;

    //Under here are all methods/functions defined
    public:

        //This is an example function
        void sort_for_freemove(){
            Serial.println("Hello World");
        }

        //Creates an array with random coordinates
        void randomStoneCoords(){
            for(int i = 0; i < stoneCount+1;i++){
                stoneCoords[i][0] = rand() % 20 + 1; //Moludus (%) operator computes the remainding result of rand()/20
                stoneCoords[i][1] = rand() % 20 + 1;
                delay(200);
            }
        }

};