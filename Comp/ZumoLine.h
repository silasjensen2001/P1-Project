//This header file contains the "Rockxanne" class with its respective attributes and methods
//This is a comment for testing GIT :O
//First a few libraries need to be imported
#include "arduino.h"
#include "ZumoDriveLine.h"



//The Rockxanne class includes all the classes
class Rockxanne: public ZumoDriveLine{    //, public ZumoCom 

    //Private members can NOT be accesed with ex. Rockxan."member" 
    //To get information about private members you need to make public methods that returns the respective value
    private:


    public:

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


        

        
};
