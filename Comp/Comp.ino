#include "ZumoLine.h"

Zumo32U4LineSensors LineSensors;

Rockxanne Rockxan;
int dist;
float coords[2] = {18,0};
float coords2[2] = {-10,-20};
float second_coords[3][2] = {{0, -30}, {16, -30}, {8, 10}};

void setup() {
    Serial.begin(9600);
    Rockxan.initAll("LCD");
    Rockxan.setLineThresh(500);
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
            Rockxan.DetectCan(4000);
            delay(50);

            dist = Rockxan.FindDistance();
            Rockxan.display_print("Dist:");
            Rockxan.display_print((String)dist, 0,1);
            delay(2500);

            if (dist <= 2){      
            } else if (dist <= 6) {
                straight_can();
            } else {
                second_can();
            }

            drive_back(); 
            delay(50);             
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
    Rockxan.koortilkordinat(coords, 22);
    Rockxan.drive_to_line();
    delay(100);
    Rockxan.setSpeeds(-100, -100);
    delay(500);
    Rockxan.setSpeeds(0, 0);
}

void second_can(){
    for(int i = 0; i<3; i++){
       Rockxan.koortilkordinat(second_coords[i], 22); 
       delay(100);
    }
    Rockxan.drive_to_line();
    delay(100);
    Rockxan.setSpeeds(-100, -100);
    delay(500);
    Rockxan.setSpeeds(0, 0);
}

void drive_back(){
    Rockxan.koortilkordinat(coords2, 25); 
    delay(50);
    Rockxan.turn_to(90, 100);  
}