//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported

#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class ZumoCom{

    //Protected er ligesom private blot at hovedeklassen har adgang til dem
    protected:

    //Under public are all functions/methods defined
    public:

        void example(){
            Serial.println("Hello world");
        }


};