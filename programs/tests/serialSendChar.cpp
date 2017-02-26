#include "device/serial/serialclass.h"
#include <stdio.h>

void printArrHex(unsigned char inArr[], int size);

int main()
{
    bool xonxoff = false;
    bool rtscts = false;
    string name = "\\\\.\\COM4"; // /dev/ttyUSB0
    int32_t baud = 115200;
    size_t parity = 0;
    size_t bits = 6;
    size_t stop = 1;


    int32_t baudArray[]= {9600,19200,25600,38400, 56000, 115200, 128000, 256000};

    for(int currBaudIndex = 0 ; currBaudIndex < 8; currBaudIndex++)//Test at various baud rates
    {
        baud = baudArray[currBaudIndex];
        Serial *myport = new Serial(name, baud, bits, parity, stop);
        myport->set_flowcontrol(rtscts, xonxoff);
        myport->set_timeout(1, 0);
        cout <<"Started baud " << baud << endl;
        cout << "going to put char" << endl;
        myport->put_char(104); // letter h
        int32_t result = myport->get_char();
            cout << "put char and get char didnt crash at baud" << baudArray[currBaudIndex] << "result is" << result << endl;
        if( 104 != (int)result)
        {
            cout << "Sending single char failed" << endl;
            return -1;
        }

        char retAlpha[128];

        int i;
        for(i = 1; i <= 127 ; i++)//Put all the characters from
        {
            myport->put_char(i);
        }

        for(i =1; i <= 127  ; i++)//Fill the retAlpha array with getChar statements
        {
            retAlpha[i]= myport->get_char();
            if(i != (int)retAlpha[i])
            {
                cout << "Concurrent ASCII table test failed" << endl;
                return -2;
            }
        }
        retAlpha[128] = '\0';
        cout <<"Passed baud " << baudArray[currBaudIndex] << endl;
    }
    return 0;
}
