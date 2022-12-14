//This header contains all the methods for steering the zumo
//It also has the variables that keep track of position and angle
//First a few libraries need to be imported
#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include "ZumoCom.h"
#include "RoutePlanner.h"


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class ZumoDrive: public ZumoCom, public RoutePlanner{

    //Private members can't be accesed with ex. Rockxan."member" 
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

        //Variables  /attributes 
        float current_pos[2];      //{x,y} [cm]  
        float current_angle;       //degrees
        float gyro_offset_z;
        float gyro_drift_z;
        float gyro_last_angle;     //degrees
        float angle_thresh;        //degrees
        float target_angle;        //degrees
        float len_rotation;        //cm
        float last_angle_error;
        float kc, kd;
        float error_sum;

        uint16_t brightness_levels[4] = { 1, 2 , 3 , 4 };

        int min_speed;             //zumo value (minimum speed needed for zumo to drive)
        int counts_rotation;
        int left_counts;
        int right_counts;
        int right_speed;
        int left_speed;
        int gyro_correction_time;   //ms

        unsigned long time_offset;
        unsigned long gyro_timer;   //ms



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

        void init_drive(){
            counts_rotation = 909;
            len_rotation = 11.5;

            Wire.begin();
            IMU.init();
            IMU.enableDefault();
            IMU.configureForTurnSensing();
            proxSensors.initThreeSensors();
            proxSensors.setBrightnessLevels(brightness_levels, 4);

            calibrate_gyro();
            reset();
        }

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
            last_angle_error = 0;
            kc = 0.5;
            kd = 15;

            Encoders.getCountsAndResetLeft();
            Encoders.getCountsAndResetRight();
        }

        void set_PD_values(float new_kc, float new_kd){
            kc = new_kc;
            kd = new_kd;
        }

        float get_P_value(){
            return kc;
        }
        float get_D_value(){
            return kd;
        }
        float get_error(){
            return error_sum;
        }

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
            current_angle += (((float)IMU.g.z - gyro_offset_z - gyro_drift_z) * 70 * dt / 1000000000);   //- (gyro_drift_z/3)


            if (current_angle < -360){
                current_angle += 360;
            } else if (current_angle > 360){
                current_angle -= 360;
            }
        }


        //Drives straight for a given distance [cm] with a given speed [cm/s]
        //Max speed is 56 cm/s
        void drive_straight(float dist, float real_speed, bool correction_active = true, float acc = 0, float deacc = 0){
            float start_at = 0;
            float acc_zumo_value = 0;
            float deacc_zumo_value = 0;
            error_sum = 0;
            float speed = applied_speed(real_speed);
            uint16_t t2 = millis();
            uint16_t t_acc = millis();
            uint16_t t_deacc = millis();

            update_angle_gyro();
    
            Encoders.getCountsAndResetLeft();
            left_counts = 0;

            //If the distance is negative it has to drive backwards aka. having negative speed values
            if(dist < 0){
                speed = -speed;
            }

            if (correction_active){
                
                target_angle = current_angle;
                last_angle_error = 0;
            }

            display_print((String)current_angle, 0,0);

            dist = (dist/len_rotation)*counts_rotation;  //we get the distance but expresed in counts


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

            time_offset = millis();

            while(abs(dist) > abs(left_counts)){
                check_obstacle();

                left_counts = Encoders.getCountsLeft();

                if(correction_active){
                    if(dist < 0){
                        gyro_correction(true);
                    } else {
                        gyro_correction(false);
                    }      
                }

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

                if (100 < millis()-t2){ //for each 100 millis we print the angle
                    display_print((String)current_angle, 0, 0);
                    //display_print((String)dist, 0, 1);
                    t2 = millis();
                }      
            }

            Motors.setSpeeds(0,0);
            //Serial.println("done");
        }

        
            //Method that turns the Zumo to a given angle relative to the Zumo's X-axis (direction of Zumo when calibrated)
        void turn_to(float end_angle, int speed = 120){
            int i = 0;
            unsigned long t2 = millis();
            
            //if(current_angle > 0){
                //end_angle = ()
            //}

            //Checks whether the Zumo already have the angle (inside the thresholds) or not
            if ((end_angle > current_angle + angle_thresh) || (end_angle < current_angle - angle_thresh)){
                //A continous loop that runs until right angle is achieved
                while(true){
                    update_angle_gyro();

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


        //Checks and corrects speed on right motor based on angle change measured by gyro
        void gyro_correction(bool driving_backward = false){
            update_angle_gyro();

            float error = target_angle - current_angle;
            error_sum += fabs(error);

            Serial.println((String)float(millis() - time_offset) + ";" + (String)error);
            //Serial.println(millis()-gyro_timer);
            //gyro_timer = millis();

            // Get motor speed difference using proportional and derivative
            // PID terms (the integral term is generally not very useful)
            float speed_difference = error*kc + (error - last_angle_error)*kd; //error / 4

            //if(driving_backward){
                //speedDifference = -speedDifference;
            //}

            last_angle_error = error;

            // Get individual motor speeds.  The sign of speedDifference
            // determines if the robot turns left or right.
            left_speed = (int16_t)left_speed - (int16_t)speed_difference;
            right_speed = (int16_t)right_speed + (int16_t)speed_difference;

            update_angle_gyro();
            // Constrain our motor speeds to be between 70 (0) and maxSpeed.
            // One motor will always be turning at maxSpeed, and the other
            // will be at maxSpeed-|speedDifference| if that is positive,
            // else it will be stationary.  For some applications, you
            // might want to allow the motor speed to go negative so that
            // it can spin in reverse.
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
            //Kalibrering af gyro (Vi ønsker 0, når den står stille, så vi skal finde et gns. offset) // TODO: ret thor skrev på dansk adddrrr
            for (uint16_t i = 0; i < 2048; i++){
                //Venter indtil den har læst en ny værdi for gyroskopet
                while(!IMU.gyroDataReady()){}

                IMU.readGyro();

                gyro_offset_z += IMU.g.z;
            }

            gyro_offset_z /= 2048;

            for (int s = 0; s < 64; s++){   //work in progress dont judge
                while (!IMU.gyroDataReady()) {}
                IMU.readGyro();

                gyro_drift_z += IMU.g.z - gyro_offset_z;
                delay(50);
            }

            gyro_drift_z /= 64;
        }
        


        void gyro_drift(){ //delete posibily 
            for (int s = 0; s < 100; s++){
                while (!IMU.gyroDataReady()) {}
                update_angle_gyro();

                Serial.println(current_angle);
                delay(100);
            }
        }

        
        //Drives the Zumo to a given {x,y} coordinat relative to 0 (point of calibration) with a given speed [cm/s]
        void drive_to_coords(float coords[2], float speed, int angle_speed = 80){ 
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
            turn_to(angle, angle_speed);
            delay(50);
            drive_straight(norm, speed);

            //Update the position
            current_pos[0] = coords[0];
            current_pos[1] = coords[1];

        }

        

        void follow_tracks( int track_size = 20){      //track_size is the distance in the cordinatesystem between the Tracks.
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
        

        void free_move(){
            int drive_to[2] = {0,0};
            nearest_neighbour();       //sorts the stone_list array so the coordinates is in the right order.
            for (size_t i = 0; i < list_size; i++){       //drives to every stone and collects them.
                drive_to[0] = stone_list[i][0];
                drive_to[1] = stone_list[i][1];
                drive_to_coords(drive_to, 20, 160);
                display_print("Collecting");
                delay(3000);
            }
            drive_to[0] = 0;
            drive_to[1] = 0;
            drive_to_coords(drive_to, 20, 160)    //returns to starting position.
        }
};