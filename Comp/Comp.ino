#include "ZumoDriveLine.h"

Zumo32U4LineSensors LineSensors;

Rockxanne Rockxan;
int pause = 25; //ms
int speed = 50; //cm/s
int speed_to_line = 40;
int angle_speed = 400; //Zumo value
int dist; 
int detected;

float coords[2] = {20,0};
float coords2[2] = {-10,-15};

float second_coords_speedy[3][2] = {{12, -7}, {12, 13}, {-8, 13}}; //{{0, -30}, {16, -30}, {16, 10}, {-8, 13}}
float second_coords_safe[3][2] = {{0, -30}, {14, -30}, {14, 12}};
float second_coords[3][2];

bool from_second = true;
bool speedy = false;

void setup() {
    Serial.begin(9600);
    Rockxan.initAll("OLED");
    Rockxan.setLineThresh(500);
}

void loop() {
    if (Rockxan.But_A.isPressed()){
        Rockxan.display_print("A = speedy");
        Rockxan.display_print("B = safe", 0, 1);
        delay(1000);

        while (true){
            if (Rockxan.But_A.isPressed()){
                delay(500);
                speed = 50;
                speed_to_line = 40;
                angle_speed = 300;
                pause = 0;
                speedy = true;
                for (int i = 0; i < 3; i++){
                    second_coords[i][0] = second_coords_speedy[i][0];
                    second_coords[i][1] = second_coords_speedy[i][1]; 
                } 
                break;
            }
            if (Rockxan.But_B.isPressed()){
                delay(1000);
                speed = 22;
                speed_to_line = 18;
                angle_speed = 100;
                pause = 100;
                speedy = false;
                for (int i = 0; i < 3; i++){
                    second_coords[i][0] = second_coords_safe[i][0];
                    second_coords[i][1] = second_coords_safe[i][1]; 
                }
                break;
            }
        }
        
        Rockxan.turn_to(90, angle_speed);

        while(true){
            if(from_second){
                delay(pause);
                Rockxan.drive_to_line(speed_to_line);
                delay(pause);
                Rockxan.drive_straight(2, 25); //5, 12

                delay(pause);

                Rockxan.turn_to(0, angle_speed); 
            }
            

            delay(pause);
            Rockxan.drive_on_line(17);
            Rockxan.reset();

            Rockxan.display_print("Emits");
            detected = Rockxan.DetectCan(6000);
            delay(pause);

            dist = Rockxan.FindDistance();
            Rockxan.display_print("Dist:");
            Rockxan.display_print((String)dist, 0,1);

            if (dist <= 2){
                Rockxan.drive_straight(-15, speed);
                from_second = false;
            } else if (dist <= 5) { // <= 5
                Rockxan.emitOn();
                delay(100);
                Rockxan.emitOff();
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
    int len = 3; // sizeof(second_coords) / sizeof(second_coords[0]);
    len = (speedy) ?  len : len+1; //short if else (ternary operator)

    for(int i = 0; i<len-1; i++){
       Rockxan.koortilkordinat(second_coords[i], speed, angle_speed); 
       delay(pause);
    }

    Rockxan.drive_to_line(speed_to_line);

    delay(pause);
    Rockxan.setAngle(-270);
    Rockxan.drive_straight(2.5, 12);
    delay(pause);

    if(speedy){
        Rockxan.koortilkordinat(second_coords[len-1], speed, angle_speed);
        delay(pause);
        Rockxan.turn_to(-90, angle_speed);
        //Rockxan.setAngle(-90); //Snyder mig udenom en bug
    } else {
        Rockxan.drive_straight(-45, speed);
        delay(pause);
        Rockxan.koortilkordinat(coords2, speed, angle_speed);
        delay(pause);
        Rockxan.turn_to(90, angle_speed);
    }

    //Rockxan.koortilkordinat(second_coords[len-1], speed, angle_speed);
    //delay(pause);
    //Rockxan.turn_to(270, angle_speed);
    //Rockxan.setAngle(-90); //Snyder mig udenom en bug
    
    
}
