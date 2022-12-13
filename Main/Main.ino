//The main program where all header files are merged
//This comment is testing branching
//This is another test
#include <Wire.h>
#include <Zumo32U4.h>
#include "Zumo.h"


Zumo32U4ButtonA But_A;
Zumo32U4ButtonB But_B;
Zumo32U4ButtonC But_C;

Rockxanne Rockxan;

float coords[5][2] = {{50, 25}, {20, 80}, {-20,80}, {50, 50}, {0,0}};

void setup(){
    Serial.begin(9600);
    Rockxan.initAll("OLED");
    Rockxan.display_print("Klar");
}


void loop(){
    if (But_A.isPressed()){
        delay(500);
        Rockxan.display_print("Following");
        Rockxan.display_print("Tracks", 0, 1);
        Rockxan.followTracks();
        Rockxan.display_print("Done");
        Rockxan.display_print("", 0, 1);
    }

    if (But_B.isPressed()){
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
        Rockxan.drive_straight(200, 40, false);
        delay(200);
        //Rockxan.gyro_drift();

        /*
        delay(500);
        Rockxan.display_print("Driving");
        Rockxan.display_print("Coords", 0, 1);
        for(int i = 0; i < 5; i++){
            Rockxan.koortilkordinat(coords[i], 30, 120);
            delay(1500);
        }
        Rockxan.turn_to(0);

        Rockxan.display_print("Driving");
        Rockxan.display_print("Done", 0, 1);
        */
    }
}
