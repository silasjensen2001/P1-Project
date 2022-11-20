//This header contains all the methods for steering the zumo
//It also has the variables that keep track of position and angle
//First a few libraries need to be imported
#include "arduino.h"
#include "ZumoComLine.h"


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class ZumoDriveLine: public ZumoComLine{

    //Private members can't be accesed with ex. Rockxan."member" 
    //To get information about private members you need to make public methods that returns the respective value
    protected:
        //Zumo objects
        Zumo32U4Encoders Encoders;
        Zumo32U4IMU IMU;
        Zumo32U4Motors Motors;
        Zumo32U4LineSensors LineSensors;
        Zumo32U4ProximitySensors ProxSensors;
        

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

        unsigned int left_counts;
        unsigned int right_counts;
        unsigned int values[5];
        uint16_t brightnessLevels[4] = { 1, 2 , 3 , 4 }; //skal sige der er 4.

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
        ZumoDriveLine(){
            
        }


        //---------------------Public methods---------------------//

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
            ProxSensors.setBrightnessLevels(brightnessLevels, 4);

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
            angle_thresh = 0.2;
            gyro_correction_time = 1;
            min_speed = 80;

            Encoders.getCountsAndResetLeft();
            Encoders.getCountsAndResetRight();
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
        //Max speed is 50 cm/s
        void drive_straight(float dist, float real_speed, bool backwards = false, bool correction_active = true, float acc = 0, float deacc = 0){
            float angle_store = current_angle;
            float start_at = 0;
            float acc_zumo_value = 0;
            float deacc_zumo_value = 0;
            float speed = applied_speed(real_speed);
            unsigned long t2 = millis();
            unsigned long t_acc = millis();
            unsigned long t_deacc = millis();

            updateAngleGyro();
            if(backwards){
                speed = -speed;
                correction_active = false;
            }

            Encoders.getCountsAndResetLeft();
            left_counts = 0;

            //Motors.setSpeeds(speed, speed);
    
            if (correction_active){
                target_angle = current_angle;
                right_speed = speed;
            }

            display_print((String)current_angle, 0,0);

            dist = phys_to_encod_dist(dist);  //we get the distance but expresed in counts


            if(acc != 0){
                acc_zumo_value = applied_speed(acc);  //zumo values pr. sec.
                left_speed = right_speed = min_speed;
            } else {
                left_speed = right_speed = speed;
            } 
            if(deacc != 0){
                deacc_zumo_value = applied_speed(deacc);                  //zumo values pr. sec.
                start_at = dist - fabs(((pow(8, 2) - pow(real_speed, 2))/(2*deacc))/len_rotation * counts_rotation);  //dist needed to deacc in counts
            }

            Motors.setSpeeds(left_speed, right_speed);

            //A continuos loop that stops running, when distance is met
            while(dist > left_counts){
                left_counts = Encoders.getCountsLeft();

                if(correction_active){
                    gyro_correction();
                }

                //Accelerates the Zumo if active
                if(acc != 0 && millis() > t_acc+100 && left_speed <= speed){
                    right_speed += int(acc_zumo_value/10);
                    left_speed += int(acc_zumo_value/10);
                    Motors.setSpeeds(left_speed, right_speed);
                    t_acc = millis();
                }

                //Deaccelerates the Zumo if active
                if(deacc != 0 && left_counts >= start_at && millis() > t_deacc+100 && left_speed > min_speed){
                    right_speed -= deacc_zumo_value/10;
                    left_speed -= deacc_zumo_value/10;
                    Motors.setSpeeds(left_speed, right_speed);
                    t_deacc = millis();
                    acc = 0;
                }

                //for each 100 millis we print the angle
                if (100 < millis()-t2){ 
                    display_print((String)deacc_zumo_value, 0, 0);
                    display_print((String)dist, 0, 1);
                    t2 = millis();
                }          
            }

            Motors.setSpeeds(0,0);
        }


        //Method that turns the Zumo to a given angle relative to the Zumo's X-axis (direction of Zumo when calibrated)
        void turn_to(float end_angle, int speed = 70){
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
                        break;
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
            //Kalibrering af gyro (Vi ønsker 0, når den står stille, så vi skal finde et gns. offset) // TODO: ret thor skrev på dansk adddrrr
            for (uint16_t i = 0; i < 2048; i++){
                //Venter indtil den har læst en ny værdi for gyroskopet
                while(!IMU.gyroDataReady()){}

                IMU.readGyro();

                gyro_offset_z += IMU.g.z;
            }

            gyro_offset_z /= 2048;

        }


        //Drives the Zumo to a given {x,y} coordinat relative to 0 (point of calibration) with a given speed [cm/s]
        void koortilkordinat(float coords[2], float speed){ 
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

            display_print((String)angle, 0, 0);
            delay(2000);

            turn_to(angle);
            display_print((String)norm, 0, 0);
            delay(2000);
            drive_straight(norm, speed);

            //Updates the position
            current_pos[0] = coords[0];
            current_pos[1] = coords[1];

        }

        void drive_to_line(){
            int i = 0;
            Motors.setSpeeds(60, 60);
            LineSensors.readCalibrated(values);

            while((values[0] < thresh) || (values[4] < thresh)){
                LineSensors.readCalibrated(values);

                if(values[0] > thresh){
                    Motors.setLeftSpeed(0);
                    i++;
                }
                updateAngleGyro();
                if (values[4] > thresh){
                    Motors.setRightSpeed(0);
                    i++;
                }
                //if(i >= 2){
                //    break;
                //}
                
            }

        }

        void drive_on_line(){
            int16_t lastError = 0;
            int i2 = 0;
            int16_t position = LineSensors.readLine(values);

            while((values[0] < thresh) || (values[4] < thresh)){
                position = LineSensors.readLine(values);

                // Our "error" is how far we are away from the center of the
                // line, which corresponds to position 2000.
                int16_t error = position - 2000;

                // Get motor speed difference using proportional and derivative
                // PID terms (the integral term is generally not very useful
                int16_t speedDifference = error + 4 * (error - lastError); //error / 4

                lastError = error;

                // Get individual motor speeds.  The sign of speedDifference
                // determines if the robot turns left or right.
                int16_t leftSpeed = (int16_t)90 + speedDifference;
                int16_t rightSpeed = (int16_t)90 - speedDifference;

                // Constrain our motor speeds to be between 0 and maxSpeed.
                // One motor will always be turning at maxSpeed, and the other
                // will be at maxSpeed-|speedDifference| if that is positive,
                // else it will be stationary.  For some applications, you
                // might want to allow the motor speed to go negative so that
                // it can spin in reverse.
                leftSpeed = constrain(leftSpeed, 70, (int16_t)120);
                rightSpeed = constrain(rightSpeed, 70, (int16_t)120);

                Motors.setSpeeds(leftSpeed, rightSpeed);
                //LineSensors.readCalibrated(values);
         
                if(values[0] > thresh){
                    Motors.setLeftSpeed(0);
                    i2++;
                }
                if (values[4] > thresh){
                    Motors.setRightSpeed(0);
                    i2++;
                }
                //if(i2 == 2){
                //    display_print((String)i2);
                //    delay(3000);
                
                //    break;
                //}
                
            }
            

            /*
            Motors.setSpeeds(0,0);

            while((values[0] < thresh))
            {
                LineSensors.readCalibrated(values);
                Motors.setLeftSpeed(70);
                updateAngleGyro();
            }
            Motors.setSpeeds(0,0);

            while((values[4] < thresh))
            {
                Motors.setRightSpeed(70);
                LineSensors.readCalibrated(values);
                updateAngleGyro();
            }
            */
            Motors.setSpeeds(0,0);
            

            //Reset position and angle every time the zumo is at receiver
            reset();
        }


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
            delay(1000);
            for(uint16_t i = 0; i < 120; i++)
            {
                if (i > 30 && i <= 90)
                {
                Motors.setSpeeds(-150, 150);  //BEfore: values -200, 200
                }
                else
                {
                Motors.setSpeeds(150, -150);
                }

                LineSensors.calibrate();
            }
            Motors.setSpeeds(0, 0);
            
        }

        int FindDistance(){
            ProxSensors.read();
            leftSensor = ProxSensors.countsFrontWithLeftLeds();
            rightSensor = ProxSensors.countsFrontWithRightLeds();
            SensorPR = rightSensor + leftSensor;

            return SensorPR;
        }

        void DetectCan(int timer){ //Detects can and stops the belt.
            unsigned long t = millis();

            LineSensors.emittersOn();
            while (leftSensor != rightSensor || leftSensor < 4){
                ProxSensors.read();
                leftSensor = ProxSensors.countsFrontWithLeftLeds();
                rightSensor = ProxSensors.countsFrontWithRightLeds();

                if(timer < millis()-t){
                    display_print((String)leftSensor, 0,0);
                    delay(1000);
                    return;
                }
            }

            LineSensors.emittersOff();
        }

        void setSpeeds(int speedLeft, int speedRight){
            Motors.setSpeeds(speedLeft, speedRight);
        }
        

};
