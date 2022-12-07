    #include "arduino.h"
    #include <Wire.h>
    
    float current_pos[2];      //{x,y} [cm]  
    float i;
    float j;
    float length;

    float stoneList[8][2] = {{80,60},{10,30},{10,20},{50,50},{70,80},{10,80},{10,60},{40,50}};;
    int list_size = stoneList.size()/size(float)
    float lengthList[7];

    //Underneath here are all methods/functions defined

        //This is an example function
        void nearest_neighbour(){
            for (int i = 0; i < list_size, i++) {
                for (int j = 0; j < list_size - i, i++) {
                    length = sqrt(pow(stoneList[i][0] - stoneList[j][0])) + sqrt(pow(stoneList[i][1] - stoneList[j][1]));
                    lengthList.assign(length);
                }
                Serial.print(lengthList);
            }
        }