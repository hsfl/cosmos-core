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
    Serial *myport = new Serial(name, baud, bits, parity, stop);
    myport->set_flowcontrol(rtscts, xonxoff);
    myport->set_timeout(1, 0);

    int32_t baudArray[]= {115200, 128000, 25600, 9600,
                         19200, 38400, 56000, 115200, 128000, 256000};

    for(int currBaudIndex = 0 ; currBaudIndex < sizeof(baudArray); currBaudIndex++)//Test at various baud rates
    {
        baud = baudArray[currBaudIndex];
        myport->put_char(104); // letter h
        int32_t result = myport->get_char();

        if( 104 != (int)result)
        {
            cout << "Sending single char failed" << endl;
            return -1;
        }

        char uAlpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //Upper case alphabet
        char retAlpha[27];

        int i = 0;
        for(i = 65; i <= 90 ; i++)
        {
            myport->put_char(i);
        }

        for(i =65; i <= 90 ; i++)
        {
            retAlpha[i-65]= myport->get_char();
        }
        retAlpha[26] = '\0';

        if(strcmp(uAlpha,retAlpha) != 0) //
        {
            cout << "Concurrent Alphabet test failed" << endl;
            cout << "uAlpha is " << uAlpha << " retAplha is " << retAlpha << endl;
            return -1;
        }
    }
    return 0;
}

void printArrHex(unsigned char inArr[], int size)
{

    int i,j;
    j = 0;
    printf("Printing array in hex...\n");
    for (i = 0; i < (size); i++)
    {
        printf("%02x",inArr[i]);
        j++;
    }
    printf("\n");
}
