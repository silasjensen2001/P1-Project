#include "ZumoLine.h"

Zumo32U4LineSensors LineSensors;

Rockxanne Rockxan;
int dist;
float coords[2] = {28,0};
float coords2[2] = {-10,-20};

void setup() {
    Serial.begin(9600);
    Rockxan.initAll("LCD");
    Rockxan.setLineThresh(600);
}

void loop() {
    if (Rockxan.But_A.isPressed()){
        delay(1000);
        Rockxan.turn_to(90, 100);
        for(int i=0; i<4; i++){
            delay(50);
            Rockxan.drive_to_line();
            delay(50);
            Rockxan.drive_straight(6, 10);

            delay(50);
            Rockxan.turn_to(0, 90);

            delay(50);
            Rockxan.drive_on_line();
            Rockxan.reset();

            Rockxan.display_print("Emits");
            Rockxan.DetectCan();
            Rockxan.display_print("DOne");
            delay(50);

            dist = Rockxan.FindDistance();
            Rockxan.display_print("Dist:");
            Rockxan.display_print((String)dist, 0,1);
            delay(50);

            Rockxan.koortilkordinat(coords, 22);
            Rockxan.display_print("DOne");  
            delay(50);
            Rockxan.koortilkordinat(coords2, 25);  
            delay(50);
            Rockxan.turn_to(90, 100);   
        }
    }

    if (Rockxan.But_B.isPressed()){
        Rockxan.calibrateSensors();
        /*
        while (true)
        {
            dist = Rockxan.FindDistance();
            Rockxan.display_print((String)dist);
            delay(500);
            if (Rockxan.But_C.isPressed()){
                Rockxan.display_print("Reset");
                break;
            }    
        } */     
    }

    if (Rockxan.But_C.isPressed()){
        //Rockxan.drive_on_line();
        Rockxan.reset();
        Rockxan.display_print("Reset");
        //Rockxan.printValues();
    }
    
}

void straight_can(){

}

void second_can(){
    
}