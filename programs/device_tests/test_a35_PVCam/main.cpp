#include <stdlib.h>
#include <stdio.h>
#include "a35acq.h"

#define IMAGEWIDTH 320
#define IMAGEHEIGHT 256
#define IMAGESIZE (IMAGEWIDTH*IMAGEHEIGHT)

typedef char Int8;
typedef short Int16;
typedef int Int32;
typedef unsigned int uInt32;

Ca35acq Camera;
unsigned short m_nPhotonFrameBuffer[IMAGESIZE];

float FramesPerSec;

static unsigned int        AcqWinWidth = IMAGEHEIGHT;
static unsigned int        AcqWinHeight = IMAGEWIDTH;

double t1 = 0, t2;
Int32 nbFrame;
uInt32 currBufNum;


int main(int argc, char *argv[])
{
    int 	        i, j, error;
    unsigned int	bitsPerPixel;
    int WaterfallBand;
    unsigned short *bufAddr = NULL;
    int MinVal, MaxVal;
    int AutoMinVal, AutoMaxVal;
    char intfName[64];
    static short *DisplayBuffPtr;

    printf("Testing A35 camera...\n\n");


    // Clear error flag
    error=0;

    // Initialize Camera
    if( Camera.InitCamera(&AcqWinWidth, &AcqWinHeight) == false) {
        exit(-73);
    }

    Camera.StartCamera();


    nbFrame = 0;
    //t1 = timeGetTime();

    long LastBuf = 0;
    bool Start = true;

    for(;;) {

        currBufNum = Camera.GetCameraFrame(m_nPhotonFrameBuffer);

        //*** Calculate the number of frames per seconds every 300 frames
        nbFrame++;
        if (nbFrame>=300) {
            //                t2 = timeGetTime();
            FramesPerSec = 1000.0 * (double)nbFrame / (double)(t2-t1);
            t1 = t2;
            nbFrame=0;
        }
    }
}
