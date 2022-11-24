#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4LineSensors.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU IMU;
Zumo32U4ButtonC buttonC;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensors; 

bool useEmitters = true;
int Speed = 100;
int brightnessLevels[6] = {0.4, 0.8, 1, 6, 7, 8}; //skal sige der er 4.
uint16_t lineSensorValues[5];
int leftSensor = proxSensors.countsFrontWithLeftLeds();
int rightSensor = proxSensors.countsFrontWithRightLeds(); 
int SensorPR = 0;

void Pri(){
Serial.println(String(leftSensor) + " " + String(rightSensor) + " " + String(SensorPR));
}

int FindDistance(){
  proxSensors.read();
  leftSensor = proxSensors.countsFrontWithLeftLeds();
  rightSensor = proxSensors.countsFrontWithRightLeds();
  SensorPR = rightSensor + leftSensor;
  Pri();
  return SensorPR;
}

void DetectCan(){ //Detects can and stops the belt.
  //lineSensors.emittersOn();
  while (true) {
    FindDistance();
    if (SensorPR>=1) {
      lineSensors.emittersOff();
      useEmitters = false;
      Serial.println("Sluk Emitters");
      //break;
  }
  }
}

void DriveStr(){
  delay(500);//this needs to be here or the program works quicker than the can is moving. returning SensorPR=1
  FindDistance();
  Serial.println(SensorPR);
  if (SensorPR >= 4) { //Can is close. drives straight.
    motors.setSpeeds(Speed, Speed);
    delay(2000); //it has to drive until it touches the line
    motors.setSpeeds(-Speed, -Speed); //and back up until it touches a new line
    delay(2000);
    Serial.println("Can is close.");
  }
  if (0<SensorPR<4){
Serial.println("Can is far away.");
  }
  motors.setSpeeds(0, 0);
}

void setup() {
  proxSensors.initThreeSensors();
  proxSensors.setBrightnessLevels(brightnessLevels, 6);
  lineSensors.initFiveSensors();
  display.clear();
   
}

void loop() {
  FindDistance();
  /*DetectCan(); //detects can and stops the belt
  DriveStr(); // drives straight if Can is close, else nothing
*/
}
