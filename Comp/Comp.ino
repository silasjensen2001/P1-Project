#include "ZumoLine.h"

Zumo32U4LineSensors LineSensors;

Rockxanne Rockxan;
int pause = 25; //ms
int speed = 40; //cm/s
int speed_to_line = 25;
int angle_speed = 200; //Zumo value
int dist; 
int detected;

float coords[2] = {20,0};
float coords2[2] = {-6,-10};
float second_coords[3][2] = {{14, -7}, {14, 17}, {-8, 20}}; //{{0, -30}, {16, -30}, {16, 10}, {-8, 13}}

bool from_second = true;

void setup() {
    Serial.begin(9600);
    Rockxan.initAll("OLED");
    Rockxan.setLineThresh(500);
}

void loop() {
    if (Rockxan.But_A.isPressed()){
        delay(1000);
        Rockxan.turn_to(90, angle_speed);
        for(int i=0; i<8; i++){
            if(from_second){
                delay(pause);
                Rockxan.drive_to_line(speed_to_line);
                delay(pause);
                Rockxan.drive_straight(5, 12);

                delay(pause);
                Rockxan.turn_to(0, angle_speed); 
            }
            

            delay(pause);
            Rockxan.drive_on_line(speed_to_line);
            Rockxan.reset();

            Rockxan.display_print("Emits");
            detected = Rockxan.DetectCan(10000);
            delay(pause);

            dist = Rockxan.FindDistance();
            Rockxan.display_print("Dist:");
            Rockxan.display_print((String)dist, 0,1);
            delay(1000);

            if (dist <= 2){
                Rockxan.drive_straight(-15, speed);
                from_second = false;
            } else if (dist <= 5) {
                straight_can();
                from_second = false;
            } else {
                second_can();
                from_second = true; 
            }

            delay(pause);             
        }
        Rockxan.display_print("Well");
        Rockxan.display_print("Done", 0, 1);
    }

    if (Rockxan.But_B.isPressed()){
        Rockxan.reset();
        Rockxan.display_print("Reset");
        //Rockxan.drive_straight(-20, 22, true);     
    }

    if (Rockxan.But_C.isPressed()){
        //Rockxan.drive_on_line();
        Rockxan.display_print("Calibrating");
        Rockxan.calibrateSensors();
        
        //Rockxan.printValues();
    }
    
}


void straight_can(){
    Rockxan.koortilkordinat(coords, speed, angle_speed);
    Rockxan.drive_to_line(speed_to_line);
    delay(pause);
    Rockxan.setAngle(0);
    Rockxan.drive_straight(-35, speed);
}


void second_can(){
    Rockxan.emitOn();
    delay(500);
    Rockxan.emitOff();

    for(int i = 0; i<2; i++){
       Rockxan.koortilkordinat(second_coords[i], speed, angle_speed); 
       delay(pause);
    }

    Rockxan.drive_to_line();
    delay(pause);
    Rockxan.setAngle(90);
    Rockxan.drive_straight(2.5, 12);
    //Rockxan.drive_straight(-5, speed);
    //delay(pause);
    //Rockxan.setSpeeds(0, 0);
    delay(pause);
    Rockxan.koortilkordinat(second_coords[2], speed, angle_speed);
    delay(pause);
    Rockxan.turn_to(270, angle_speed);
    Rockxan.setAngle(-90); //Snyder mig udenom en bug
}
