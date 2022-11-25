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

void setup(){
    Serial.begin(9600);
    Rockxan.initAll("OLED");
    Rockxan.display_print("Klar");
}

void loop(){
    if (But_A.isPressed()){
        Rockxan.followTracks();
    }

    if (But_B.isPressed()){
        delay(500);
        Rockxan.drive_straight(200, 40, true, 12, 12);
    }

    if (But_C.isPressed()){
        delay(700);
        Rockxan.reset();
        Rockxan.drive_straight(200, 20);
        delay(500);
        Rockxan.turn_to(-180);
        Rockxan.drive_straight(200, 20);
    }
    delay(500);
    Serial.println("her går det godt");
}
