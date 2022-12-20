//This header contains all the methods for steering the zumo
//It also has the variables that keep track of position and angle
//First a few libraries need to be imported
#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include "ZumoCom.h"
#include "RoutePlanner.h"


//The Zumodrive class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class ZumoDrive: public ZumoCom, public RoutePlanner{

    //Private members can't be accesed with ex. ZumoDrive."member" 
    //To get information about private members you need to make public methods that returns the respective value
    protected:
        //Zumo objects
        Zumo32U4Encoders Encoders;
        Zumo32U4IMU IMU;
        Zumo32U4Motors Motors;
        Zumo32U4ProximitySensors proxSensors;
        Zumo32U4ButtonA But_A;
        Zumo32U4ButtonB But_B;
        Zumo32U4ButtonC But_C;

        //Attributes
        float current_pos[2];      //{x,y} [cm]  
        float current_angle;       //degrees
        float gyro_offset_z;
        float gyro_last_angle;     //degrees
        float angle_thresh;        //degrees
        float target_angle;        //degrees
        float len_rotation;        //cm
        float last_angle_error;
        float kc, kd, ki;
        float error_sum;
        float integral_error;

        double gyro_drift_z;       

        int min_speed;             //zumo value (minimum speed needed for zumo to drive)
        int counts_rotation;
        int left_counts;
        int right_counts;
        int right_speed;
        int left_speed;
        int gyro_correction_time;   //ms

        uint16_t brightness_levels[4] = {1, 2 , 3 , 4 }; //uin16_t makes it compatible with the function proxSensors.setBrightnessLevels()

        unsigned long gyro_timer;   //ms


        //This can be uncommented and use to test different PID values
        /*
        unsigned long time_offset;
        float PIDvalues[9][3] = {{1.5, 19, 25}, 
                        {1.5, 19, 26}, 
                        {1.5, 19, 27}, 
                        {1.5, 19, 28}, 
                        {1.5, 19, 29}, 
                        {1.5, 20, 30}, 
                        {1.5, 21, 35}, 
                        {1.5, 18, 40}, 
                        {1.5, 17, 45}};
        */


        //---------------------Protected methods---------------------//


        //Translates zumo speed values to physical speed [cm/s]
        float real_speed(float value){
            return (0.1454*value-2.1536);
        }

        //Translates physical speed [cm/s] to zumo speed values 
        float applied_speed(float speed){
            return (speed+2.1536)/0.1454;
        }


    public:

        //The constructor
        ZumoDrive(){
            
        }


        //---------------------Public methods---------------------//

        //Initializes everything relevant inside the ZumoDrive class
        void init_drive(){
            Wire.begin();
            IMU.init();
            IMU.enableDefault();
            IMU.configureForTurnSensing();
            proxSensors.initThreeSensors();
            proxSensors.setBrightnessLevels(brightness_levels, 4);

            set_PID_values(1.5, 19, 23);
            calibrate_gyro();
            reset();
        }

        //Init_all has to be called once in the setup() in main.ino
        //Initializes all inherited classes
        //Takes parameter "OLED" or "LCD"
        void init_all(String display = "OLED"){
            init_display(display);               

            display_print("Ik Klar", 0, 0);

            init_drive();
            
            display_print("Klar", 0, 0);
        }

        //Turns of the motors if obstacle is detected and turns them back on when obstacle is gone
        //Is called during the drive_straight() function
        void check_obstacle(){
            proxSensors.read();
            int right_sensor = proxSensors.countsFrontWithRightLeds();
            int left_sensor = proxSensors.countsFrontWithLeftLeds();

            while(left_sensor >= 4 or right_sensor >= 4){
                proxSensors.read();
                right_sensor = proxSensors.countsFrontWithRightLeds();
                left_sensor = proxSensors.countsFrontWithLeftLeds();
                Motors.setSpeeds(0, 0);
            }
            Motors.setSpeeds(left_speed, right_speed);
        }
        
        //Resets all relevant attributes
        void reset(){
            current_pos[0] = 0;
            current_pos[1] = 0;      
            current_angle = 0;        
            left_counts = 0;
            right_counts = 0;
            angle_thresh = 0.2;
            gyro_correction_time = 1;
            min_speed = 70;
            last_angle_error = 0;
            counts_rotation = 909;
            len_rotation = 11.5;

            Encoders.getCountsAndResetLeft();
            Encoders.getCountsAndResetRight();
        }
        
        //Can set new PID values and takes the respective values as parameters
        void set_PID_values(float new_kc, float new_ki, float new_kd){
            kc = new_kc;
            kd = new_kc*new_kd;
            ki = new_kc*(1/new_ki);
        }

        //Returns the proportional gain
        float get_P_value(){
            return kc;
        }
        //Returns the derivative gain
        float get_D_value(){
            return kd/kc;
        }
        //Returns the integral gain
        float get_I_value(){
            return kc/ki;
        }
        //Returns the error_sum and i used under PID-testing
        float get_error(){
            return error_sum;
        }


        //Can be uncommented to test for different PID-values (is commented out to save memory)
        /*
        void test_PID(){
            for (int i = 0; i < 7; i++){
                set_PID_values(PIDvalues[i][0], PIDvalues[i][1], PIDvalues[i][2]);
                drive_straight(200, 30);
                Serial.println("new;" + (String)get_P_value() + ";" + (String)get_D_value() + ";" + (String)get_I_value());
                delay(3000);
        }

        Serial.println("done");
        }
        */
        
        
        //This function updates the angle according to changes in the gyroscope
        //The function has to be called very often to work well, since it is calculated
        //from movements
        void update_angle_gyro() {
            static uint16_t last_update = 0;
            uint16_t m = micros();
            uint16_t dt = m - last_update;
            last_update = m;

            IMU.readGyro();

            //The angle change can now be calculated
            //Every raw reading can be translated to degrees with the factor 0,07 deg/s
            //Datasheet https://www.pololu.com/file/0J731/L3GD20H.pdf)
            current_angle += (((float)IMU.g.z - gyro_offset_z - gyro_drift_z*dt) * 70 * dt / 1000000000);   //- (gyro_drift_z/3)

            //Ensure that the angle will not become less than -360 and more than 360
            if (current_angle < -360){
                current_angle += 360;
            } else if (current_angle > 360){
                current_angle -= 360;
            }
        }


        //Drives straight for a given distance [cm] with a given speed [cm/s]
        //Max speed is 56 cm/s
        //Optionally gyro-PID correction can be disabled
        //Acceleration and deaccelaeration can be specified. 
        //When 0 (default) the zumo will use the desired speed instantly    
        void drive_straight(float dist, float real_speed, bool correction_active = true, float acc = 0, float deacc = 0, bool use_prox = true){
            float start_at = 0;
            float acc_zumo_value = 0;
            float deacc_zumo_value = 0;
            float speed = applied_speed(real_speed);
            uint16_t t2 = millis();
            uint16_t t_acc = millis();
            uint16_t t_deacc = millis();

            error_sum = 0;
            integral_error = 0;
            left_counts = 0;

            dist = (dist/len_rotation)*counts_rotation;  //we get the distance but expresed in counts

            update_angle_gyro();
            Encoders.getCountsAndResetLeft();
            

            //If the distance is negative it shall drive backwards
            if(dist < 0){
                speed = -speed;
            }

            if (correction_active){   
                target_angle = current_angle;
                last_angle_error = 0;
            }

            display_print((String)current_angle, 0,0);

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

            
            //Can be uncommented when PID-values are tested
            //bool rise_done = false;
            //current_angle -= 3.5;
            //time_offset = millis();

            while(abs(dist) > abs(left_counts)){
                if(use_prox){
                    check_obstacle();  
                }
                

                left_counts = Encoders.getCountsLeft();

                if(correction_active){
                    if(dist < 0){
                        gyro_correction(true);
                    } else {
                        gyro_correction(false);
                    }      
                }
                
                update_angle_gyro();

                if(acc != 0 && millis() > t_acc+100 && left_speed <= speed){
                    right_speed += int(acc_zumo_value/10);
                    left_speed += int(acc_zumo_value/10);
                    Motors.setSpeeds(left_speed, right_speed);
                    t_acc = millis();
                }
                if(deacc != 0 && left_counts >= start_at && millis() > t_deacc+100 && left_speed > min_speed){
                    right_speed -= deacc_zumo_value/10;
                    left_speed -= deacc_zumo_value/10;
                    Motors.setSpeeds(left_speed, right_speed);
                    t_deacc = millis();
                    acc = 0;
                }
                
                update_angle_gyro();

                //for each 100 millis we print the angle
                if (100 < millis()-t2){ 
                    display_print((String)current_angle, 0, 0);
                    t2 = millis();
                } 

                //Can be uncommented when PID values are tested
                /*
                if ((abs(dist)/2 < abs(left_counts)) && rise_done != true){
                    current_angle += 3;
                    rise_done = true;
                }
                */     
            }

            Motors.setSpeeds(0,0);
            //Serial.println("done");
        }

        
        //Method that turns the Zumo to a given angle relative to the global X-axis (direction of Zumo when calibrated)
        //Optionally the desired speed can be specified (default = 120)
        void turn_to(float end_angle, int speed = 120){
            int i = 0;
            unsigned long t2 = millis();
            target_angle = end_angle;
            float angle_diff = end_angle - current_angle;

            //Determines whether it should turn right or left
            if(fabs(angle_diff) > 180){
                if (current_angle < 0){
                    current_angle = 360 + current_angle;
                } else {
                    current_angle = -360 + current_angle;
                }
            }


            //A continous loop that runs until right angle is achieved
            //It uses simple bang-bang an to prevent too much overshoot from momentum
            //the desired angle must be met through over 10 iterations
            while(i != 10){
                update_angle_gyro();

                if (50 < millis()-t2){
                    display_print((String)current_angle, 0, 0);
                    t2 = millis();
                }
                
                if (current_angle > end_angle-angle_thresh && current_angle < end_angle+angle_thresh){
                    Motors.setSpeeds(0, 0);
                    delay(20);
                    speed = 70;
                    i++; 
                } else if (current_angle > end_angle-angle_thresh) {
                    Motors.setSpeeds(speed, -speed);
                } else if (current_angle < end_angle+angle_thresh) {
                    Motors.setSpeeds(-speed, speed);
                }
            }
            update_angle_gyro();
        }


        //Checks and corrects speed on right motor based on angle change measured by gyro
        //If the Zumo is set with negativ speed values the driving_backward must be set true (false on default)
        void gyro_correction(bool driving_backward = false){
            update_angle_gyro();

            float error = target_angle - current_angle;
            
            integral_error += error;

            //Can be uncommented when testing for PID
            //Serial.println((String)float(millis() - time_offset) + ";" + (String)error);
            //error_sum += fabs(error);

            //Get motor speed difference using a PID-controller
            //PID terms (the integral term is generally not very useful)
            float speed_difference = error*kc + ki * integral_error + kd*(error - last_angle_error); 

            last_angle_error = error;

            // Get individual motor speeds.  The sign of speedDifference
            // determines if the robot turns left or right.
            left_speed = (int16_t)left_speed - (int16_t)speed_difference;
            right_speed = (int16_t)right_speed + (int16_t)speed_difference;

            update_angle_gyro();

            //When driving backwards Constrain our motor speeds to be between -400 (maxspeed) and -70 (above -70 the motors stop)
            //Constrain our motor speeds to be between 70 (below 70 the motors stop) and 400 (maxSpeed)
            if(driving_backward){
                left_speed = constrain(left_speed, -400, (int16_t)-70);
                right_speed = constrain(right_speed, -400, (int16_t)-70);
            }else{
                left_speed = constrain(left_speed, 70, (int16_t)400);
                right_speed = constrain(right_speed, 70, (int16_t)400);
            }          

            Motors.setSpeeds(left_speed, right_speed);

            update_angle_gyro();   
        }

        
        //The gyro needs be calibrated
        //This is due to an offset reading and a drifting value
        void calibrate_gyro(){
            for (uint16_t i = 0; i < 2048; i++){
                //Venter indtil den har læst en ny værdi for gyroskopet
                while(!IMU.gyroDataReady()){}

                IMU.readGyro();

                gyro_offset_z += IMU.g.z;
            }

            gyro_offset_z /= 2048;


            unsigned long time = micros(); // = millis();
            unsigned long dt;
             
            for (int s = 0; s < 64; s++){   //work in progress dont judge
                while (!IMU.gyroDataReady()) {}
                IMU.readGyro();


                dt = micros() - time;
                time = micros();
                gyro_drift_z += (IMU.g.z - gyro_offset_z)/dt;
                delay(50);
            }

            gyro_drift_z = gyro_drift_z/64;
        }
        

        //Drives the Zumo to a given {x,y} coordinat relative to 0 (point of calibration) with a given speed [cm/s]
        //Optionally the velocity (in zumo values) when turning, acc-, and deaccelartion can be specified
        void drive_to_coords(float coords[2], float speed, int angle_speed = 80, float acc = 0.0, float de_acc = 0.0, bool use_prox = true){ 
            //This is used to calculate the angle the Zumo has to turn to get in the direction of the point
            //It is the formular for finding the angle between two vectors
            int x_vec[2] = {1, 0};
            int x = coords[0] - current_pos[0];
            int y = coords[1] - current_pos[1];

            float norm = sqrt((pow(x, 2) + pow(y, 2)));

            float dot_prod = x_vec[0]*x + x_vec[1]*y;
            float norm_prod = sqrt((pow(x_vec[0], 2) + pow(x_vec[1], 2))) * sqrt((pow(x, 2) + pow(y, 2)));

            float angle = acos(dot_prod/norm_prod)*180/M_PI;   //is multiplied by 180/pi to go from radians to degrees. 

            //The formular above does not take sign into account
            if (current_pos[1] > coords[1]){
                angle = -angle;
            }
            
            //Now it can turn the angle and drive the length
            if (int(angle) == 180 || int(angle) == -180){
                drive_straight(-norm, speed, true, acc, de_acc, use_prox);
            } else {
                turn_to(angle, angle_speed);
                delay(50);
                drive_straight(norm, speed, true, acc, de_acc, use_prox); 
            }
            

            //Update the position
            current_pos[0] = coords[0];
            current_pos[1] = coords[1];
        }

        
        //This functions drives to stonecoordinats by using tracks
        //The length (cm) between the tracks can be specified (20 on default)
        //This function assumes a rectangular field
        void follow_tracks(int track_size = 20, bool use_prox = true){     
            sort_xy();     
            int x_track = 0;             //the track it goes to
            float drive_to[2] = {0,0};   //the destination we want to go to next
            bool on_track = false;       //If false; next stone is closest to the next Track/ If true next stone is on the Track
            for (int i = 0; i < list_size; i++){     //repeates the loup until it is done for all the stones. 
                drive_to[0] = stone_list[i][0];    // er lige blevet flyttet fra over for løkken. 
                drive_to[1] = stone_list[i][1];
                if ((stone_list[i][0] % track_size) == 0){  //If this stone is on the Track.
                    x_track = stone_list[i][0];
                    if (on_track == false){            //this if statement is runned trough if the stone is on another track than the one just used.
                        drive_to[0] = x_track;
                        drive_to[1] = 0; 
                        drive_to_coords(drive_to, 20, 160); //Drives to the Track.
                }}
                else if ((stone_list[i][0] % track_size) <= track_size/2){ //When the stone is placed on the right side of the Track closet to it. Note: if the stone is just between to rows it will be collected from the row to the left.
                    x_track = stone_list[i][0] - (stone_list[i][0] % track_size);
                    if (on_track == false){
                        drive_to[0] = x_track;
                        drive_to[1] = 0;
                        drive_to_coords(drive_to, 20, 160); //Drives to the Track.
                }}
                else { //Wheen the stone is closest to the Track to the right.
                    x_track = stone_list[i][0] + (track_size - (stone_list[i][0] % track_size));
                    if (on_track == false){
                        drive_to[0] = x_track;
                        drive_to[1] = 0;
                        drive_to_coords(drive_to, 20, 160); //Drives to the Track.
                }}
                delay(200);

                // Zumo is at the Track and y-position is y = 0.
                drive_to[0] = x_track;
                drive_to[1] = stone_list[i][1]; //defines the GoTo y-value to the stones y-value.
                drive_to_coords(drive_to, 20, 160); //Drives up the track to the y-position. 

                delay(200);
                // Zumo is on the Track on the same y-position as the stone.
                if (x_track != stone_list[i][0]){ //If the stone does not lay on the track.
                    drive_to[0] = stone_list[i][0];
                    drive_to_coords(drive_to, 20, 160); //The stone is reached.

                    display_print("Collecting");
                    delay(3000);
                    //collects stone.
                    drive_to[0] = x_track; //Drives back to the track, but same y-position.
                    drive_to_coords(drive_to, 20, 160);
                }            
                else{ //if the stone is on track.
                    //Collecting stone.
                    display_print("Collecting");
                    delay(3000);
                }
                //The stone is collected and we are on the track at the y-position of the stone.
                if (i == list_size - 1){ //if its the last stone, it drives down the track to the y-position=0.
                    drive_to[0] = x_track;
                    drive_to[1] = 0;
                    drive_to_coords(drive_to, 20, 160);  //Drives down to y-position = 0. 
                }
                else if ((((x_track - (track_size/2))) < stone_list[i+1][0] ) && (stone_list[i+1][0]<= (x_track + (track_size/2)))){  //If the next stone is located closest to the current track. 
                    on_track = true;
                }
                else { // If the next stone is closest to another track.
                    on_track = false;
                    drive_to[0] = x_track;
                    drive_to[1] = 0;
                    drive_to_coords(drive_to, 20, 160);
                }
            }
            drive_to[0] = 0;  
            drive_to[1] = 0; 
            drive_to_coords(drive_to, 20, 160);   //drives to origo (0,0).
        }
        
        //Drives directly to coordinates without taking tracks into account
        //Sorts the stone list by using nearest neighbour (nn) or farthest insertion (fi)
        void free_move(String sort_function = "nn"){
            if (sort_function == "nn"){
                nearest_neighbour(); 
            } else {
                farthest_insertion();
            }             
            
            float drive_to[2] = {0,0};

            //drives to every stone and collects them.
            for (size_t i = 0; i < list_size; i++){       
                drive_to[0] = stone_list[i][0];
                drive_to[1] = stone_list[i][1];
                drive_to_coords(drive_to, 20, 160);
                display_print("Collecting");
                delay(3000);
            }
            
            //return to starting position.
            drive_to[0] = 0;
            drive_to[1] = 0;
            drive_to_coords(drive_to, 20, 160);    
        }
        
};