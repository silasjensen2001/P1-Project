//The main program where all header files are merged
//This comment is testing branching
//This is another test
#include <Wire.h>
#include <Zumo32U4.h>
#include "Zumo.h"
//Hej

Zumo32U4ButtonA But_A;
Zumo32U4ButtonB But_B;
Zumo32U4ButtonC But_C;

Rockxanne Rockxan;

void setup(){
    Serial.begin(9600);
    Rockxan.init_all("OLED");  //If its the OLED version write that, else write LCD.
    Rockxan.display_print("Klar");
}


void loop(){
    if (But_A.isPressed()){      //Initiates Tracs following.
        delay(500);
        Rockxan.display_print("Following");
        Rockxan.display_print("Tracks", 0, 1);
        Rockxan.follow_tracks();
        Rockxan.display_print("Done");
        Rockxan.display_print("", 0, 1);
    }

    if (But_B.isPressed()){         //The Zumo drives straight and returns to the starting position.
        delay(500);
        Rockxan.display_print("Driving");
        Rockxan.display_print("Straight", 0, 1);
        Rockxan.drive_straight(120, 35, true, 15, 15); 
        delay(500);
        Rockxan.drive_straight(-120, 35);
        Rockxan.display_print("Done");
        Rockxan.display_print("", 0, 1);
    }

    if (But_C.isPressed()){
        delay(500);
    }
}
