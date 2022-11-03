#include <iostream>
#include <fstream>
//#include "SimpleSerial.h"

using namespace std;  //std = standard and includes functions such as cin, cout, open, etc.

/*
char com_port[] = "\\\\.\\COM6";
DWORD COM_BAUD_RATE = CBR_9600;
SimpleSerial Serial(com_port, COM_BAUD_RATE);

int reply_wait_time = 7;
string syntax_type = "mine";

string incoming = Serial.ReadSerialPort(reply_wait_time, syntax_type);
int x;
*/

int main( int argc, char* argv[] )
{
    /*
    while (true) {
        string incoming = Serial.ReadSerialPort(reply_wait_time, syntax_type);
        cout << incoming + "\n";
        cout << "Continue?";
        cin >> x;

        if (x == 0){
            Serial.CloseSerialPort();
            break;
        }
    }
    */


    
    std::ofstream myfile;
    myfile.open ("example.csv");
    myfile << "This is the first cell in the first column.\n";
    myfile << "a,b,c,\n";
    myfile << "c,s,v,yo\n";
    myfile << "1,2,3.456\n";
    myfile << "semi;colon";
    myfile.close();
    return 0;
    
}