#include <Wire.h>
#include <Zumo32U4.h>
#include "ZumoLine.h"



Zumo32U4LineSensors LineSensors;


Rockxanne Rockxan;

void setup() {
    Serial.begin(9600);
    Rockxan.initAll("LCD");
    Rockxan.setLineThresh(800);
}

void loop() {
    if (Rockxan.But_A.isPressed()){
       Rockxan.drive_to_line();
    }

    if (Rockxan.But_B.isPressed()){
        Rockxan.calibrateSensors();
    }

    if (Rockxan.But_C.isPressed()){
        Rockxan.reset();
        //Rockxan.printValues();
    }
    
}