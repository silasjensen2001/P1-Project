//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported

#include "arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>


//The Rockxanne class includes all useful methods for steering the zumo robot
//Its attributes keep track of variables like current angle and position
class ZumoCom{

    //Protected er ligesom private blot at hovedeklassen har adgang til dem
    protected:
        bool oled = false;
        Zumo32U4LCD LCD;
        Zumo32U4OLED OLED;
        

    //Under public are all functions/methods defined
    public:

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