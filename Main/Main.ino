//The main program where all header files are merged
//This comment is testing branching
//This is another test
#include <Wire.h>
#include <Zumo32U4.h>
#include "ZumoDrive.h"

Zumo32U4ButtonA But_A;
Zumo32U4ButtonB But_B;
Zumo32U4ButtonC But_C;

ZumoDrive Rockxan;

//Setup function is an built-in arduino that must be run
//It used to initialize the program
void setup(){
    Serial.begin(9600);
    Rockxan.init_all("OLED");  //If its the OLED version write that, else write LCD.
    Rockxan.display_print("Klar");
}


//The loop function is another built-in function
//As the name suggest the function is a continous loop
void loop(){

    //If button A is pressed the track following function is run
    if (But_A.isPressed()){      
        delay(500);
        Rockxan.follow_tracks(20, false);
    }

    //If B is pressed the the drive straight function is demonstrated with acceleration and gyro correction
    if (But_B.isPressed()){  
        
    }

    //Button C starts the freemove function
    //The coordinates are sorted using neares neighbour
    if (But_C.isPressed()){
        
    }
}

