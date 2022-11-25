//This header contains all the methods for steering the zumo
//It also has the variables that keep track of position and angle
//First a few libraries need to be imported
#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class Rockxanne{

    //Private members can't be accesed with ex. Rockxan."member" 
    //To get information about private members you need to make public methods that returns the respective value
    protected:
        //Zumo objects
        Zumo32U4Encoders Encoders;
        Zumo32U4IMU IMU;
        Zumo32U4Motors Motors;
        Zumo32U4LineSensors LineSensors;
        Zumo32U4ProximitySensors ProxSensors;
        Zumo32U4OLED OLED;
        Zumo32U4LCD LCD;
        

        //Variables  /attributes 
        float current_pos[2];      //{x,y} [cm]  
        float current_angle;       //degrees
        float gyro_offset_z;
        float gyro_last_angle;     //degrees
        float angle_thresh;        //degrees
        float target_angle;        //degrees
        float len_rotation;        //cm
        
        int min_speed;             //zumo value (minimum speed needed for zumo to drive)
        int counts_rotation;
        int right_speed;
        int left_speed;
        int gyro_correction_time;   //ms
        int leftSensor;
        int rightSensor;
        int SensorPR;
        int thresh;
        int left_counts;
        int right_counts;

        unsigned int values[5];
        uint16_t brightnessLevels[4] = {0.6, 1, 7, 8}; //skal sige der er 4.//uint16_t brightnessLevels[4] = {0.4, 0.8, 7, 8}; //skal sige der er 4.
        int size = 4;

        bool oled = false;

        //unsigned long gyro_timer;   //ms



        //---------------------Protected methods---------------------//


        //Translates zumo speed values to physical speed [cm/s]
        float real_speed(float value){
            return (0.1454*value-2.1536);
        }

        //Translates physical speed [cm/s] to zumo speed values 
        float applied_speed(float speed){
            return (speed+2.1536)/0.1454;
        }

        //Translates physical distance to encoder counts
        int phys_to_encod_dist(float distance){
            return (distance/len_rotation)*counts_rotation;
        }

        //Translates encoder counts to pysical distance in cm
        float encod_to_phys_dist(int encoder_counts){
            return encoder_counts/counts_rotation*len_rotation;
        }


    public:
        Zumo32U4ButtonA But_A;
        Zumo32U4ButtonB But_B;
        Zumo32U4ButtonC But_C;

        //The constructor
        Rockxanne(){
            
        }

        //---------------------Public methods---------------------//

        //initAll has to be called once in the setup() in main.ino
        //initializes all inherited classes
        void initAll(String display = "OLED"){
            //This init is found in ZumoCom.h
            initDisplay(display);               //takes parameter "OLED" or "LCD"

            display_print("Ik Klar", 0, 0);

            //This init is found in "ZumoDrive.h"
            initDrive();
            
            display_print("Klar", 0, 0);
        }


        void initDrive(){
            counts_rotation = 909;
            len_rotation = 11.5;

            Wire.begin();
            IMU.init();
            IMU.enableDefault();
            IMU.configureForTurnSensing();

            LineSensors.initFiveSensors();
            ProxSensors.initFrontSensor();

            thresh = 400;
            leftSensor = ProxSensors.countsFrontWithLeftLeds();
            rightSensor = ProxSensors.countsFrontWithRightLeds(); 
            SensorPR = 0;
            ProxSensors.setBrightnessLevels(brightnessLevels, size);

            calibrate_gyro();
            reset();
        }

        
        //resets pos, angle and encoder counts
        void reset(){
            current_pos[0] = 0;
            current_pos[1] = 0;      
            current_angle = 0;        
            left_counts = 0;
            right_counts = 0;
            angle_thresh = 0.4;
            gyro_correction_time = 1;
            min_speed = 80;

            Encoders.getCountsAndResetLeft();
            Encoders.getCountsAndResetRight();
        }

        void setAngle(float angle){
            current_angle = angle;
        }


        //This function updates the angle according to changes in the gyroscope
        //The function has to be called very often to work well, since it is calculated
        //from movements
        void updateAngleGyro() {
            static uint16_t last_update = 0;
            uint16_t m = micros();
            uint16_t dt = m - last_update;
            last_update = m;

            IMU.readGyro();

            //The angle change can now be calculated
            //Every raw reading can be translated to degrees with the factor 0,07 deg/s
            //Datasheet https://www.pololu.com/file/0J731/L3GD20H.pdf)
            current_angle += (((float)IMU.g.z - gyro_offset_z) * 70 * dt / 1000000000);   

            if (current_angle < -360){
                current_angle += 360;
            } else if (current_angle > 360){
                current_angle -= 360;
            }
        }


        //Drives straight for a given distance [cm] with a given speed [cm/s]
        //Max speed is 50 cm/s and min. speed is 8 cm/s (depending on battery level)
        void drive_straight(float dist, float real_speed, bool correction_active = true, float acc = 0, float deacc = 0){
            //Convert physical speed in cm to 
            float speed = applied_speed(real_speed);

            //Used to time a print
            unsigned long t2 = millis();

            //Calculate the vectorvalues for the motion
            float target_posX = cos((current_angle*M_PI/180))*dist;
            float target_posY = sin((current_angle*M_PI/180))*dist;

            updateAngleGyro();

            //If the distance is negative it has to drive backwards aka. having negative speed values
            if(dist < 0){
                speed = -speed;
                correction_active = false;
            }

            //Reset encoder and the left_counts attribute
            Encoders.getCountsAndResetLeft();
            left_counts = 0;
    
            //If Gyro correction is activated we want to set the desired angle to current angle
            if (correction_active){
                target_angle = current_angle;
                right_speed = speed;
            }

            display_print((String)current_angle, 0,0);

            //Convert physical distance in cm to distance in encoder counts
            dist = phys_to_encod_dist(dist);  

            left_speed = right_speed = speed;

            //Start driving
            Motors.setSpeeds(left_speed, right_speed);

            //A continuos loop that stops running, when distance is met
            while(abs(dist) > abs(left_counts)){
                left_counts = Encoders.getCountsLeft();
                
                if(correction_active){
                    gyro_correction();
                }

                //for each 100 millis we print the angle
                if (100 < millis()-t2){ 
                    display_print((String)current_angle, 0, 1);
                    t2 = millis();
                }       
            }   

            //Stop driving
            Motors.setSpeeds(0,0);

            //Update position
            current_pos[0] = current_pos[0] + target_posX;
            current_pos[1] = current_pos[1] + target_posY;
        }


        //Method that turns the Zumo to a given angle relative to the Zumo's X-axis (direction of Zumo when calibrated)
        void turn_to(float end_angle, int speed = 120){
            int i = 0;
            unsigned long t2 = millis();

            //Checks whether the Zumo already have the angle (inside the thresholds) or not
            if (end_angle > current_angle + angle_thresh or end_angle < current_angle-angle_thresh){
                //A continous loop that runs until right angle is achieved
                while(true){
                    updateAngleGyro();

                    if (100 < millis()-t2){
                        display_print((String)current_angle, 0, 0);
                        t2 = millis();
                    }

                    if (current_angle > end_angle-angle_thresh && current_angle < end_angle+angle_thresh){
                        Motors.setSpeeds(0, 0);
                        delay(20);
                        speed = 70;
                        i++;
                        if(i>3){
                            break;    
                        }    
                    } else if (current_angle > end_angle-angle_thresh) {
                        Motors.setSpeeds(speed, -speed);
                    } else if (current_angle < end_angle+angle_thresh) {
                        Motors.setSpeeds(-speed, speed);
                    }
                }
            }
        }


        //Checks and corrects speed on right motor to drive straight based on angle change measured by gyro
        void gyro_correction(){
            updateAngleGyro();
            float d_angle = current_angle - gyro_last_angle;

            if (abs(d_angle) > angle_thresh){   //
                if (current_angle > target_angle) {
                    right_speed--;          
                } else if (current_angle < target_angle) {
                    right_speed++; 
                }

                Motors.setRightSpeed(right_speed);
                //gyro_timer = millis(); 
                gyro_last_angle = current_angle;
            } 
        
            updateAngleGyro();   
            
        }


        //The gyro needs be calibrated
        //This is due to an offset reading and a drifting value
        void calibrate_gyro(){
            for (uint16_t i = 0; i < 2048; i++){
                //Waits until a new value is available from the gyro
                while(!IMU.gyroDataReady()){}

                IMU.readGyro();

                gyro_offset_z += IMU.g.z;
            }

            gyro_offset_z /= 2048;

        }


        //Drives the Zumo to a given {x,y} coordinat relative to 0 (point of calibration) with a given speed [cm/s]
        void koortilkordinat(float coords[2], float speed, int angle_speed = 80){ 
            //This is used to calculate the angle the Zumo has to turn to get in the direction of the point
            //It is the formular for finding the angle between two vectors
            int x_vec[2] = {1, 0};
            int x = coords[0] - current_pos[0];
            int y = coords[1] - current_pos[1];
            float norm = sqrt((pow(x, 2) + pow(y, 2)));

            float dot_prod = x_vec[0]*x + x_vec[1]*y;
            float norm_prod = sqrt((pow(x_vec[0], 2) + pow(x_vec[1], 2))) * sqrt((pow(x, 2) + pow(y, 2)));

            float angle = acos(dot_prod/norm_prod)*180/M_PI;

            //The formular above does not take sign into account
            if (current_pos[1] > coords[1]){
                angle = -angle;
            }
            
            //Now it can turn the angle and drive the length
            turn_to(angle, angle_speed);
            delay(50);
            drive_straight(norm, speed);

            //Update the position
            current_pos[0] = coords[0];
            current_pos[1] = coords[1];

        }


        void drive_to_line(int speedCM = 15){
            int i = 0;
            int speed = applied_speed(speedCM);

            Motors.setSpeeds(speed, speed);
            LineSensors.readCalibrated(values);

            while((values[0] > thresh) || (values[4] > thresh)){
                LineSensors.readCalibrated(values);
                
                //This avoids a false reading to stop the robot
                if(i>=2){
                    Motors.setSpeeds(speed, speed);
                    i=0;
                } 

                if(values[0] < thresh){
                    Motors.setLeftSpeed(0);
                    i++;
                }
                if (values[4] < thresh){
                    Motors.setRightSpeed(0);
                    i++;
                } 
                
                updateAngleGyro(); 
            }
            Motors.setSpeeds(0,0);

        }

        void drive_on_line(int speedCM){
            int16_t lastError = 0;
            int i3 = 0;
            int i2 = 0;
            int16_t position = LineSensors.readLine(values, true, true);
            int speed = applied_speed(speedCM);

            while((values[0] > thresh) || (values[4] > thresh)){
                position = LineSensors.readLine(values, true, true);

                // Our "error" is how far we are away from the center of the
                // line, which corresponds to position 2000.
                int16_t error = position - 2000;

                // Get motor speed difference using proportional and derivative
                // PID terms (the integral term is generally not very useful
                int16_t speedDifference = error*0.8 + 1*(error - lastError); //error / 4

                lastError = error;

                // Get individual motor speeds.  The sign of speedDifference
                // determines if the robot turns left or right.
                int16_t leftSpeed = (int16_t)speed + speedDifference;
                int16_t rightSpeed = (int16_t)speed - speedDifference;

                // Constrain our motor speeds to be between 0 and maxSpeed.
                // One motor will always be turning at maxSpeed, and the other
                // will be at maxSpeed-|speedDifference| if that is positive,
                // else it will be stationary.  For some applications, you
                // might want to allow the motor speed to go negative so that
                // it can spin in reverse.
                leftSpeed = constrain(leftSpeed, 70, (int16_t)400);
                rightSpeed = constrain(rightSpeed, 70, (int16_t)400);

                Motors.setSpeeds(leftSpeed, rightSpeed);
                //LineSensors.readCalibrated(values);
         
                if(values[0] < thresh){
                    Motors.setLeftSpeed(0);
                    Motors.setRightSpeed(70);
                    i2++;
                }
                if (values[4] < thresh){
                    Motors.setRightSpeed(0);
                    Motors.setLeftSpeed(70);
                    i2++;
                }

                if(i2 == 2 && i3 == 0){
                    drive_straight(-2, 20);
                    Motors.setSpeeds(70, 70);
                    delay(100);
                    i2 = 0;
                    i3++;
                    values[4] = 1000;
                }

                
            }
            drive_straight(0.1,10);
            Motors.setSpeeds(0,0);
            

            //Reset position and angle every time the zumo is at receiver
            reset();
        }

        //Good function for debbugging
        void printValues(){
            int pos;
            while(true){
                //LineSensors.readCalibrated(values);
                pos = LineSensors.readLine(values);
                //display_print((String)values[4], 0,0);
                display_print((String)pos, 0,0);
                delay(100);
                if(But_B.isPressed()){
                    break;
                }
            }
        }

        //Sets the threshhold for linesensing
        void setLineThresh(uint16_t val){
            thresh = val;
        }
        
        //Calibrates the sensors
        void calibrateSensors(){
            // Wait 1 second and then begin automatic sensor calibration
            // by rotating in place to sweep the sensors over the line
            delay(500);
            for(uint16_t i = 0; i < 120; i++)
            {
                if (i > 30 && i <= 90)
                {
                Motors.setSpeeds(-200, 200);  //BEfore: values -200, 200
                }
                else
                {
                Motors.setSpeeds(200, -200);
                }

                LineSensors.calibrate();
            }
            Motors.setSpeeds(0, 0);
            
        }

        //Reads the front prox. sensor with the right and left led
        //The two values are then added together and returned as a measure for the distance
        int FindDistance(){
            ProxSensors.read();
            leftSensor = ProxSensors.countsFrontWithLeftLeds();
            rightSensor = ProxSensors.countsFrontWithRightLeds();
            SensorPR = rightSensor + leftSensor;

            return SensorPR;
        }

        //Detects can and stops the belt.
        int DetectCan(int timer){ 
            unsigned long t = millis();
            LineSensors.emittersOn();

            ProxSensors.read();
            leftSensor = ProxSensors.countsFrontWithLeftLeds();
            rightSensor = ProxSensors.countsFrontWithRightLeds();

            while (leftSensor != rightSensor || leftSensor < 1){
                LineSensors.emittersOn();
                delay(50);
                ProxSensors.read();
                LineSensors.emittersOn();
                leftSensor = ProxSensors.countsFrontWithLeftLeds();
                rightSensor = ProxSensors.countsFrontWithRightLeds();

                if(timer < millis()-t){
                    display_print((String)leftSensor, 0,0);
                    return 0;
                }
            }

            LineSensors.emittersOff();
            return 1;
        }


        void setSpeeds(int speedLeft, int speedRight){
            Motors.setSpeeds(speedLeft, speedRight);
        }

        void emitOn(){
            LineSensors.emittersOn();
        }

        void emitOff(){
            LineSensors.emittersOff();
        }


        //The communication channel has to be initialized and is dependent on whether OLED or LCD is used
        void initDisplay(String OLED_or_LCD){
            Wire.begin();

            if (OLED_or_LCD == "OLED"){
                OLED.clear();
                OLED.setLayout8x2();
                oled = true;
                
            } else if (OLED_or_LCD == "LCD"){
                oled = false;
                
            }
        }

        //This functions prints to the display
        //It takes into consideration whether a OLED or LCD is used
        void display_print(String text, int posX = 0, int posY = 0){
            if (oled) {
                OLED.gotoXY(posX,posY);
                OLED.print("        ");
                OLED.gotoXY(posX,posY);
                OLED.print(text);
            } else {
                LCD.gotoXY(posX,posY);
                LCD.print("        ");
                LCD.gotoXY(posX,posY);
                LCD.print(text);
            }
        }

};
