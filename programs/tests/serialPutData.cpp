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
        vector<uint8_t> sendData;
        sendData.push_back(104);
        myport->put_data(sendData); // letter h
        vector<uint8_t> recievedData;
        myport->get_data(recievedData, sendData.size());
//        for (i = 0; i < myVector.size(); i++)
//            {
//                (*myVector[i]).displaydata();
//            }
        if( 104 != (int)recievedData[0])
        {
            cout << "Sending single char via put_data and get_data failed" <<(int) recievedData[0] << endl;
            return -1;
        }

        sendData.pop_back();
        recievedData.pop_back();

        int i = 0;
        for(i = 65; i <= 90 ; i++)
        {
            sendData.push_back(i);
        }
        myport->put_data(sendData);
        myport->get_data(recievedData, sendData.size());


        if(sendData != recievedData ) //
        {
            cout << "Concurrent Alphabet test for put_data failed" << endl;
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
