#include <Wire.h>
#include <Zumo32U4.h>
#include "arduino.h"

Zumo32U4Encoders Encoders;
Zumo32U4IMU IMU;
Zumo32U4LCD LCD;
Zumo32U4Motors Motors;
Zumo32U4ButtonA But_A;
Zumo32U4ButtonB But_B;
Zumo32U4ButtonC But_C;

double right_encoder = 0;
double left_encoder = 0;
double right_speed = 100;
double left_speed = 100; 

float gyro_offset_z;
float angle = 0;
float target_angle = 0;
float off_angle = 0.4;

float gyro_drift_a = 0;
float gyro_drift_b = 0;
float drift = 0;

float l_omgang = 11.7; //længden pr omgang i cm

float sten[4][2] = {{4,4},{6,6},{9,9},{11,11}}; //stenenes kordinater.
float position[1][2] = {{0,0}}; //robottens position.

int i = 0;
int usten = 0;
int omgange = 0;
int maalOmgange = 0;
int antalSten = 3;
int posili = 0;

bool drive = false;
bool from_wayoff = false;





void setup(){
  Serial.begin(9600);
  LCD.print("Press A to start");
}

void koortilkordinat(int posili){
    while (position[0][posili]<sten[usten][posili]-0.1){
      Motors.setSpeeds(left_speed, right_speed);
      delay(50);
      Motors.setSpeeds(0, 0);
      position[0][posili] += 0.1;
      LCD.clear();
      LCD.print(position[0][posili]);
      Serial.println("posili");
      Serial.println(posili);
      Serial.println(position[0][posili]);
      Serial.println(sten[usten][posili]);
    }
    LCD.clear();
}

void gotox(){ //får robotten tilbage til x=0
  while (position[0][0]>0.1){
    Motors.setSpeeds(left_speed, right_speed);
    delay(50);
    Motors.setSpeeds(0, 0);
    position[0][0] -= 0.1;
    LCD.clear();
    LCD.print(position[0][posili]);
    Serial.println(position[0][0]);
}
}


void findEnSten(int posili){
  koortilkordinat(posili);
  posili++;
  Motors.setSpeeds(-left_speed, right_speed); //her skal der drejes 90 grader mod venstre:
  delay(990);
  Motors.setSpeeds(0, 0);
  koortilkordinat(posili);
  LCD.print("sten opsamles");
  delay(5000);
  LCD.clear();
  Motors.setSpeeds(-left_speed, right_speed); //her skal der drejes 180 grader mod venstre:
  delay(1800);
  posili = 0;
  gotox();
  Motors.setSpeeds(-left_speed, right_speed); //her skal der drejes 90 grader mod venstre:
  delay(990); //nu er den klar til at finde næste sten
}

void loop(){
  if (But_A.isPressed()){
  LCD.clear();
  for (int usten = 0; usten<=antalSten; usten++){
    findEnSten(posili);
    Serial.println("usten");
    Serial.println(usten);
    Serial.println("omgange ");
    Serial.println(omgange);
    Serial.println("maalOmgange ");
    Serial.println(maalOmgange);
    Serial.println("antalSten ");
    Serial.println(antalSten);
    Serial.println("posili ");
    Serial.println(posili);
  }
  Serial.println("done");
  delay(5000000);
  }
}