#include <iostream>
#include<windows.h>

//#include <string.h>

//std = standard and includes functions such as cin, cout, open, etc.
using namespace std;  


float stone[4][2] = {{80,80},{10,80},{10,10},{50,50}};

void SortXY(){
    float UpdateList[4][2]; //another array lokal for this method/function.

    for(int i = 0; i < 4; i++){ //sets the array UpList=sten.
        UpdateList[i][0] = stone[i][0];
        UpdateList[i][1] = stone[i][1];
    }

    for (int i = 0; i < 3; i++){   //makes it check trough the array numStone-1 times.
        for (int i1 = 0; i1 < (3-i); i1++){  //Checks trough the array comparing 2 parts, beside each other, of the array each time. 
            if ((stone[i1][0] > stone[i1+1][0]) || ((stone[i1][0] == stone[i1+1][0]) && ((stone[i1][1] > stone[i1+1][1])))){
                UpdateList[i1][0] = stone[i1+1][0];
                UpdateList[i1][1] = stone[i1+1][1];
                
                UpdateList[i1+1][0] = stone[i1][0];
                UpdateList[i1+1][1] = stone[i1][1];  


                for(int i2 = 0; i2 < 4; i2++){
                    stone[i2][0] = UpdateList[i2][0];
                    stone[i2][1] = UpdateList[i2][1];                   
                } 

                Sleep(500);
                cout << stone[i1][0];
            }  
        }   
        cout << "   ";
    } 

} 

int main( int argc, char* argv[] )
{ 
    for(int i = 0; i < 4; i++){
        cout << stone[i][0]; 
        cout << ", ";
        cout << stone[i][1];
        cout << "\n";

    } 

    SortXY();
    cout << "\n\n";

    for(int i2 = 0; i2 < 4; i2++){
        
        cout << stone[i2][0]; 
        cout << ", ";
        cout << stone[i2][1];
        cout << "\n";

    }  
    
}

