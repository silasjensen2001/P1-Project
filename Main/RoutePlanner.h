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

    //Under here are all methods/functions defined
    public:

        //This is an example function
        void sort_for_freemove(){
            Serial.println("Hello World");
        }


};