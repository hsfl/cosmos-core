#include <termios.h> // terminal io (serial port) interface
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <assert.h>

#include <iostream>

#define PI (3.141592653589793)


typedef int ComPortHandle;
typedef unsigned char Byte;


using std::cout;
using std::cerr;
using std::endl;

int TestByteOrder()
{
   short int word = 0x0001;
   char *byte = (char *) &word;
   return(byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}


/*----------------------------------------------------------------------
 * convert2short
 * Convert two adjacent bytes to an integer.
 *
 * parameters:  buffer : pointer to first of two buffer bytes.
 * returns:     the converted value aa a signed short -32 to +32k.
 *--------------------------------------------------------------------*/
short convert2short(unsigned char* buffer) {
    short x;
    if(TestByteOrder() != BIG_ENDIAN) {
       x = (buffer[0] <<8) + (buffer[1] & 0xFF);
    }else{
        x = (short)buffer;
    }
    return x;
}
/*----------------------------------------------------------------------
 * convert2ushort
 * Convert two adjacent bytes to a short.
 *
 * parameters:  buffer : pointer to first of two buffer bytes.
 * returns:     the converted value as a unsigned short 0-64k.
 *--------------------------------------------------------------------*/
unsigned short convert2ushort(unsigned char* buffer) {
    unsigned short x;
    if(TestByteOrder() != BIG_ENDIAN) {
       x = (buffer[0] <<8) + (buffer[1] & 0xFF);
    }else{
        x = (unsigned short)buffer;
    }
    return x;
}

/*----------------------------------------------------------------------
 * FloatFromBytes
 * Converts bytes to Float.
 *
 * parameters:  pBytes : received buffer containing pointer to 4 bytes
 *
 * returns:     a float value.
 *--------------------------------------------------------------------*/
float FloatFromBytes(const unsigned char* pBytes)
{
    float f = 0;
    if(TestByteOrder() != BIG_ENDIAN) {
       ((Byte*)(&f))[0] = pBytes[3];
       ((Byte*)(&f))[1] = pBytes[2];
       ((Byte*)(&f))[2] = pBytes[1];
       ((Byte*)(&f))[3] = pBytes[0];
    }else{
       ((Byte*)(&f))[0] = pBytes[0];
       ((Byte*)(&f))[1] = pBytes[1];
       ((Byte*)(&f))[2] = pBytes[2];
       ((Byte*)(&f))[3] = pBytes[3];
    }

    return f;
}

/*----------------------------------------------------------------------
 * i3dmgx2_Checksum
 * Calculate checksum on a received data buffer.
 *
 * Note: The last two bytes, which contain the received checksum,
 *       are not included in the calculation.
 *
 * parameters:  buffer : pointer to the start of the received buffer.
 *              length - the length (in chars) of the buffer.
 *
 * returns:     the calculated checksum.
 *--------------------------------------------------------------------*/
unsigned short i3dmgx2_Checksum(const unsigned char* pBytes, int count)
{
    unsigned short i3_checksum = 0;
    int i = 0;

    for(i = 0; i < count; ++i)
    {
        i3_checksum+=pBytes[i];
    }

    return i3_checksum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// utility functions for working with a com port in Linux
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


//=======================================================================================
// Purge
//---------------------------------------------------------------------------------------
// Clears the com port's read and write buffers
//=======================================================================================
bool Purge(ComPortHandle comPortHandle)
{
    if (tcflush(comPortHandle,TCIOFLUSH)==-1)
    {
        cerr << "flush failed" << endl;
        return false;
    }

    return true;
}


//=======================================================================================
// OpenComPort
//---------------------------------------------------------------------------------------
// Opens a com port with the correct settings for communicating with a MicroStrain
// 3DM-GX2 sensor
//=======================================================================================
ComPortHandle OpenComPort(const char* comPortPath)
{
    ComPortHandle comPort = open(comPortPath, O_RDWR | O_NOCTTY);

    if (comPort== -1) //Opening of port failed
    {
        cerr << "Unable to open com Port\n"
                "Error:(" << errno << ") " << strerror(errno) << endl;
        return -1;
    }

    //Get the current options for the port...
    termios options;
    tcgetattr(comPort, &options);

    //set the baud rate to 115200
    int baudRate = B115200;
    cfsetospeed(&options, baudRate);
    cfsetispeed(&options, baudRate);

    //set the number of data bits.
    options.c_cflag &= ~CSIZE;  // Mask the character size bits
    options.c_cflag |= CS8;

    //set the number of stop bits to 1
    options.c_cflag &= ~CSTOPB;

     //Set parity to None
    options.c_cflag &=~PARENB;

    //set for non-canonical (raw processing, no echo, etc.)
    options.c_iflag = IGNPAR; // ignore parity check close_port(int
    options.c_oflag = 0; // raw output
    options.c_lflag = 0; // raw input

    //Time-Outs -- won't work with NDELAY option in the call to open
    options.c_cc[VMIN]  = 0;   // block reading until RX x characers. If x = 0, it is non-blocking.
    options.c_cc[VTIME] = 10;   // Inter-Character Timer -- i.e. timeout= x*.1 s

    //Set local mode and enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    //Purge serial port buffers
    Purge(comPort);

    //Set the new options for the port...
    int status=tcsetattr(comPort, TCSANOW, &options);

    if (status != 0) //For error message
    {
        cerr << "Configuring comport failed" << endl;
        return status;
    }

    //Purge serial port buffers
    Purge(comPort);

    return comPort;
}


//=======================================================================================
// CloseComPort
//---------------------------------------------------------------------------------------
// Closes a port that was previously opened with OpenComPort
//=======================================================================================
void CloseComPort(ComPortHandle comPort)
{
    close(comPort);
}

//=======================================================================================
// readComPort
//---------------------------------------------------------------------------------------
// read the specivied number of bytes from the com port
//=======================================================================================
int readComPort(ComPortHandle comPort, Byte* bytes, int bytesToRead)
{
    int bytesRead = read(comPort, bytes, bytesToRead);
}

//=======================================================================================
// writeComPort
//---------------------------------------------------------------------------------------
// send bytes to the com port
//=======================================================================================
int writeComPort(ComPortHandle comPort, const Byte* bytesToWrite, int size)
{
    return write(comPort, bytesToWrite, size);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// functions for communicationg with the 3dm-gx1
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


struct EulerAngles
{
    float roll;
    float pitch;
    float yaw;
};


//=======================================================================================
// ReadEulerAngles
//=======================================================================================
bool ReadEulerAngles(ComPortHandle comPort, EulerAngles& eulerAngles)
{
    static const Byte COMMAND_BYTE  = 0xCE;
    writeComPort(comPort, &COMMAND_BYTE, 1);

    static const int RESPONSE_SIZE = 19;
    Byte response[RESPONSE_SIZE] = {0};
    int size = readComPort(comPort, &response[0], RESPONSE_SIZE);


    //cout << size << endl;
    //cout << RESPONSE_SIZE << endl;
    //must get all the bytes we want, or it's not a valid read
    if(size != RESPONSE_SIZE)
    {
        cerr << "Invalid response size" << endl;
        return false;
    }

    // verify first byte matches the command byte
    if(response[0] != COMMAND_BYTE)
    {
        cout << response[0] << COMMAND_BYTE << endl;
        cerr << "Invalid response" << endl;
        return false;
    }

    //Verify the checksum
    short responseChecksum = convert2ushort(&response[RESPONSE_SIZE-2]);
    short calculatedChecksum = i3dmgx2_Checksum(&response[0], RESPONSE_SIZE-2);

    if(calculatedChecksum != responseChecksum)
    {
        cerr << "calculatedChecksum" << calculatedChecksum << endl;
        cerr << "responseChecksum"  <<  responseChecksum << endl;
        cerr << "Invalid Checksum"  << endl;
        return false;
    }

    //conversion factor used to convert the returned values to degrees
    static const float SCALE_AS_DEGREES = 180.0/PI;

    eulerAngles.roll  = FloatFromBytes(&response[1])*SCALE_AS_DEGREES;
    eulerAngles.pitch = FloatFromBytes(&response[5])*SCALE_AS_DEGREES;
    eulerAngles.yaw   = FloatFromBytes(&response[9])*SCALE_AS_DEGREES;

    return true;
}


//=======================================================================================
// main
//=======================================================================================
int main(int argc, char* argv[])
{
    //open the comport
    printf("Opening comport...\n");
    ComPortHandle comPort = OpenComPort("/dev/ttyUSB0");
    if(comPort > 0)
    {

        //every second get the euler angles from the sensor, and print the results
        cout<<"Roll,Pitch,Yaw"<<endl;
        while(true)
        {
            sleep(1);
            EulerAngles eulerAngles;
            if(ReadEulerAngles(comPort, eulerAngles))
            {
                cout << eulerAngles.roll << "," << eulerAngles.pitch << "," << eulerAngles.yaw << endl;
                //printf("Roll:%0.2f Pitch:%0.2f Yaw:%0.2f\n", eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw);
            }
        }


        CloseComPort(comPort);
    }


    return 0;
}


