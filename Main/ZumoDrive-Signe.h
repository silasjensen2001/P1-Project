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
        int min_speed;             //zumo value (minimum speed needed for zumo to drive)
       

        int counts_rotation;
        int left_counts;
        int right_counts;
        int right_speed;
        int left_speed;
        int gyro_correction_time;   //ms
       

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

        void initDrive(){
            counts_rotation = 909;
            len_rotation = 11.5;

            Wire.begin();
            IMU.init();
            IMU.enableDefault();
            IMU.configureForTurnSensing();

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
            current_angle += (((float)IMU.g.z - gyro_offset_z - (gyro_drift_z/3)) * 70 * dt / 1000000000);   //- (gyro_drift_z/3)


            if (current_angle < -360){
                current_angle += 360;
            } else if (current_angle > 360){
                current_angle -= 360;
            }
        }


        //Drives straight for a given distance [cm] with a given speed [cm/s]
        //Max speed is 50 cm/s
        void drive_straight(float dist, float real_speed, bool correction_active = true, float acc = 0, float deacc = 0){
            float angle_store = current_angle;
            float start_at = 0;
            float acc_zumo_value = 0;
            float deacc_zumo_value = 0;
            float speed = applied_speed(real_speed);
            uint16_t t2 = millis();
            uint16_t t_acc = millis();
            uint16_t t_deacc = millis();

            updateAngleGyro();
    
            Encoders.getCountsAndResetLeft();
            left_counts = 0;

            //If the distance is negative it has to drive backwards aka. having negative speed values
            if(dist < 0){
                speed = -speed;
            }

            if (correction_active){
                gyro_timer = millis();
                target_angle = current_angle;
                right_speed = speed;
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

            while(abs(dist) > abs(left_counts)){
                left_counts = Encoders.getCountsLeft();

                if(correction_active){
                    gyro_correction();
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
                

                if (100 < millis()-t2){ //for each 100 millis we print the angle
                    display_print((String)deacc_zumo_value, 0, 0);
                    display_print((String)dist, 0, 1);
                    t2 = millis();
                }

                  
            }

            Motors.setSpeeds(0,0);
        }


        void turn_to(float end_angle){
            int speed = 70;
            unsigned long t2 = millis();


            if (end_angle > current_angle + angle_thresh or end_angle < current_angle-angle_thresh){
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


        //Checks and corrects speed on right motor based on angle change measured by gyro
        void gyro_correction(){
            updateAngleGyro();
            float d_angle = current_angle - gyro_last_angle;

            if (gyro_correction_time < (millis()-gyro_timer) && abs(d_angle) > angle_thresh){   //
                if (current_angle > target_angle) {
                    right_speed--;          
                } else if (current_angle < target_angle) {
                    right_speed++; 
                }

                Motors.setRightSpeed(right_speed);
                gyro_timer = millis(); 
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

            for (int s = 0; s < 9; s++){   //work in progress dont judge
                while (!IMU.gyroDataReady()) {}
                IMU.readGyro();

                gyro_drift_z += IMU.g.z - gyro_offset_z;
                delay(50);
            }

            gyro_drift_z /= 10;
        }



        void gyro_drift(){ //delete posibily 
            for (int s = 0; s < 100; s++){
                while (!IMU.gyroDataReady()) {}
                updateAngleGyro();

                Serial.println(current_angle);
                delay(100);
            }
        }



        void koortilkordinat(float coords[2], float speed){ //Speed = cm/s
            int x_vec[2] = {1, 0};
            int x = coords[0] - current_pos[0];
            int y = coords[1] - current_pos[1];
            float norm = sqrt((pow(x, 2) + pow(y, 2)));

            float dot_prod = x_vec[0]*x + x_vec[1]*y;
            float norm_prod = sqrt((pow(x_vec[0], 2) + pow(x_vec[1], 2))) * sqrt((pow(x, 2) + pow(y, 2)));

            float angle = acos(dot_prod/norm_prod)*180/M_PI;

            if (current_pos[1] > coords[1]){
                angle = -angle;
            }

            display_print((String)angle, 0, 0);
            delay(2000);

            turn_to(angle);
            drive_straight(norm, speed);

            current_pos[0] = coords[0];
            current_pos[1] = coords[1];

        }

        void followTracks(){
            SortXY();
            int TrackSize= 10; //the distance in the cordinatesystem between the Tracks.
            int XTrack = 0; //the track it goes to
            float DriveTo[2] = {0,0};   //the destination we want to go to next
            koortilkordinat(DriveTo,10);
            for (size_t i = 0; i < 4; i++){
                if (((stone[i][0] % TrackSize) == 0) && (DriveTo[1] != 0)){  //If X-value is on the Track then life is simple.
                DriveTo[0] = stone[i][0];
                DriveTo[1] = 0; 
                koortilkordinat(DriveTo, 10); //obs med fart?? Vi kører derhen.
                }
                else if (((stone[i][0] % TrackSize) <= TrackSize/2) && (DriveTo[1] != 0)){ //når x er på tættest eller midt imellem en x-række der er lavere end x-værdien selv.
                    XTrack = stone[i][0] - (stone[i][0] % TrackSize);
                    DriveTo[0] = XTrack;
                    DriveTo[1] = 0;
                    koortilkordinat(DriveTo, 10); //obs med fart??
                }
                else if (DriveTo[1] != 0){ //når x er på tættest på en x-række med højere værdi end x-værdien.
                    XTrack = stone[i][0] + (stone[i][0] % TrackSize);
                    DriveTo[0] = XTrack;
                    DriveTo[1] = 0;
                    koortilkordinat(DriveTo, 10); //obs med fart?? kører hen til rækken
                } // nu er vi kørt hen til rækken men y = 0. 
                DriveTo[1] = stone[i][1]; //sætter y=punktet til y-værdi.
                koortilkordinat(DriveTo, 10); //kører op til y-værdien. 
                // vi er kørt op så vi er ud for punktet/på punktet
                if (XTrack != stone[i][0]){ //hvis stenen ikke ligger på Rækken
                    DriveTo[0] = stone[i][0];
                    koortilkordinat(DriveTo,10); //nu er punktet nået.
                    //opsamler sten.
                    DriveTo[0] = XTrack; //kører tilbage til linjen.
                    koortilkordinat(DriveTo, 10);
                }            
                else{
                    //sten opsamling.
                }//stennen er samlet op og vi er tilbage på rækken ved vores y-værdi.
                if ((i=3) || (stone[i][0]!=stone[i+1][0]) || (((stone[i][0] % TrackSize) <= TrackSize/2) != ((stone[i+1][0] % TrackSize) <= TrackSize/2))){
                    DriveTo[1] = 0;
                    koortilkordinat(DriveTo, 10);           
                }
                }
                DriveTo[0] = 0; 
                koortilkordinat(DriveTo,10);
            }
};