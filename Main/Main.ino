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

float sten[4][2] = {{80,80},{10,80},{10,10},{50,50}};


void setup(){
    Serial.begin(9600);
    Rockxan.initAll("LCD");

}

void loop(){
    if (But_A.isPressed()){
        int i = 0;
        delay(500);

        while(i < 4){
            Rockxan.koortilkordinat(sten[i], 25);
            delay(500); 
            i++;
        }       
    }

    if (But_B.isPressed()){
        delay(500);
        Rockxan.gyro_drift();
    }

    if (But_C.isPressed()){
        delay(700);
        Rockxan.reset();
        Rockxan.drive_straight(200, 20);
        delay(500);
        Rockxan.turn_to(-180);
        Rockxan.drive_straight(200, 20);
    }
}
