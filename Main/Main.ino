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

float values[9][3] = {{1.24, 137, 39}, 
                        {4, 137, 39}, 
                        {12.56, 8, 7.76}, 
                        {1.24, 80, 20}, 
                        {1.24, 137, 39}, 
                        {13.86, 7, 6.52}, 
                        {18.64, 8, 6.45}, 
                        {30, 9, 16}, 
                        {13.51, 15, 8.07}};

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
        for (int i = 0; i < 5; i++){
            Rockxan.set_PID_values(values[i][0], values[i][1], values[i][2]);
            Rockxan.drive_straight(200, 30);
            Serial.println("new;" + (String)Rockxan.get_P_value() + ";" + (String)Rockxan.get_D_value() + ";" + (String)Rockxan.get_I_value());
            delay(1000);
        }

        Serial.println("done");
    }
}
