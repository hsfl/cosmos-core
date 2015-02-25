#include "configCosmos.h"
#include <time.h>
// #include <unistd.h>
#include "sinclair_lib.h"


//void sendComboCommand(sinclair_state *handle, FILE *FP1, FILE *FP2);
void sendComboDIAGCommand(sinclair_state *handle);
void sendPollingSequence(sinclair_state *handle);


int main(int argc, char *argv[])
{
	sinclair_state handle;
	int32_t iretn;

    iretn = sinclair_stt_connect(argv[1],0x11,0x0E, &handle);

    if (iretn < 0) {
        printf("Error: sinclair_rw_connect() %d\n",iretn);
        exit (1);
    }

	nsp_ping(&handle);
	printf("%s\n\n\n",(char *)handle.mbuf.data);


	sinclair_disconnect(&handle);

}

void sendComboCommand(sinclair_state *handle, FILE *FP1, FILE *FP2)
{
//    char buf[4096];
    char filebuf[1024];
    int size;
//    uint8_t command[4];
    int16_t DetTemp;
    uint16_t ProcTemp;
    uint32_t statusBits;
    uint16_t darkOffsets[16];
    uint8_t VddCoreSetPoint;
    uint8_t MPUSetPoint;
    double AttQ[4];
    double VelAng[3];
    char ReturnCode;
    double EpochTime;
    int8_t nspOffset = -2;  // The nsp_get_data seems to be off by 2 bytes.  If that routine changes, this may be changed

    printf("Combo\n");

    // Send COMBINATION command 0x00001E0B
//    command[0] = 0x0B;
//    command[1] = 0x7E;
//    command[2] = 0x00;
//    command[3] = 0x00;

    // Get the result of the Combination Command
	sinclair_stt_result_operational result;
	size = nsp_stt_combination(handle, &result, STT_GO_COMMAND|STT_GO_LOAD_NAND|STT_GO_NOT_OFF|STT_GO_NO_RESET|STT_GO_ON, 0x00007e);

    // Print out the result in HEX
    printf("Bytes read=%d\n " , size);
	for(int i=0 ; i<size ; i++) printf("%02X," , handle->mbuf.data[i]) ; printf("\n\n");

    // Save the binary information.
	fwrite(handle->mbuf.data, sizeof(char), size, FP1);



    // Print Atitude, Velocity, and status.
    printf("Attitude Vel, Stat\n-----------\n");


	EpochTime = uint32from((uint8_t*)&handle->mbuf.data[STT_EPOCH_TIME+nspOffset],ORDER_LITTLEENDIAN);
    printf("Time: %f\n" , EpochTime);

	ReturnCode = uint32from((uint8_t*)&handle->mbuf.data[STT_RETURN_CODE+nspOffset],ORDER_LITTLEENDIAN);
    printf("ReturnCode: 0x%08X\n" , ReturnCode);


    printf("Quaternions: ");
    for(int i=0 ; i<4 ; i++) {
		AttQ[i] = doublefrom((uint8_t*)&handle->mbuf.data[STT_ATT_OFFSET + i*8 + nspOffset],ORDER_LITTLEENDIAN);
        printf("%f, " , AttQ[i]);
    }

    printf("\nVelocity: ");
    for(int i=0 ; i<3 ; i++) {
		VelAng[i] = doublefrom((uint8_t*)&handle->mbuf.data[STT_VEL_OFFSET + i*8 + nspOffset],ORDER_LITTLEENDIAN);
        printf("%f, " , VelAng[i]);
    }


    sprintf(filebuf, "Time:%f : Att:%f,%f,%f,%f : Vel:%f,%f,%f : Code:0x%08X\n" ,
                EpochTime, AttQ[0], AttQ[1], AttQ[2], AttQ[3],
                VelAng[0], VelAng[1], VelAng[2], ReturnCode);

    fwrite(filebuf, sizeof(char), strlen(filebuf), FP2);

    printf("\n");



    printf("Hardware telemetry\n-----------\n");

    // Decode and print detector temperature.
	DetTemp = int16from((uint8_t*)&handle->mbuf.data[STT_DETTEMP_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);
    printf("DetTemp = %02X = %d dec --> DetTemp = %6.1f\n" , DetTemp, DetTemp, (double)(DetTemp>>4)/16.0);

    // Decode and print processor temperature.
	ProcTemp = uint16from((uint8_t*)&handle->mbuf.data[STT_PROCTEMP_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);
    printf("ProcTemp = %02X = %d dec --> ProcTemp = %6.1f\n" , ProcTemp, ProcTemp, 25.0+(ProcTemp-0x32) * 1.5);

    // Get status bitfield.
	statusBits = uint32from((uint8_t*)&handle->mbuf.data[STT_STATUS_BITFIELD+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);
    printf("Status Bitfield = 0x%08X\n" , statusBits);

    // Get Vdd core set point
	VddCoreSetPoint = handle->mbuf.data[STT_VDDCORE_SETPOINT_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset];
    printf("VddCoreSetPoint = 0x%02X\n" , VddCoreSetPoint);

    // Get MPU set point
	MPUSetPoint = handle->mbuf.data[STT_MPU_SETPOINT_OFFSET+STT_HARDWARE_TELEMETRY_OFFSET+nspOffset];
    printf("MPUSetPoint = 0x%02X\n" , MPUSetPoint);

	printf("Reserved:%02X,%02X\n" , handle->mbuf.data[0x16+70], handle->mbuf.data[0x17+70]);

    // Print 16 dark offset values
    printf("darkOffsets:\n");
    for(int i=0 ; i<16 ; i++) {
		darkOffsets[i] = uint16from((uint8_t*)&handle->mbuf.data[STT_DARKOFFSETS_OFFSET + i*2 + STT_HARDWARE_TELEMETRY_OFFSET+nspOffset],ORDER_LITTLEENDIAN);
        printf("%d, " , darkOffsets[i]);
        if( (i==4)||(i==9) ) printf("\n");
    }
    printf("\n\n");
}


void sendComboDIAGCommand(sinclair_state *handle)
{
//char buf[4096];
int size;
//uint8_t command[4];
int Offset;
float Current, vBus, vCore, vMPU, vIO, SupervisorTemp, vSupervisor, vDetector;

    printf("Combo DIAG\n----------\n");

    // Send COMBINATION DIAG command 0x0008001B
//    command[0] = 0x1B;
//    command[1] = 0x00;
//    command[2] = 0x08;
//    command[3] = 0x00;
	handle->mbuf.size = 4;


    // Send COMBINATION command
	sinclair_stt_result_operational result;
	size = nsp_stt_combination(handle, &result, STT_GO_TEST|STT_GO_LOAD_NAND|STT_GO_NOT_OFF|STT_GO_NO_RESET|STT_GO_ON, 0x000800);
    printf("Bytes read=%d\n" , size);
	//for(int i=0 ; i<size ; i++) printf("%02X," , handle->mbuf.data[i]) ; printf("\n\n");
    //printf("%d, " , AttQ[i]);

    // Display results
    Offset = 0x82;
	Current = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0],ORDER_LITTLEENDIAN); printf("Current = %f\n" , Current);
	vBus = floatfrom((uint8_t*)&handle->mbuf.data[Offset+4],ORDER_LITTLEENDIAN); printf("vBus = %f\n" , vBus);
	vCore = floatfrom((uint8_t*)&handle->mbuf.data[Offset+8],ORDER_LITTLEENDIAN); printf("vCore = %f\n" , vCore);
	vMPU = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0xC],ORDER_LITTLEENDIAN); printf("vMPU = %f\n" , vMPU);
	vIO = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0x10],ORDER_LITTLEENDIAN); printf("vIO = %f\n" , vIO);
	SupervisorTemp = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0x14],ORDER_LITTLEENDIAN); printf("SupervisorTemp = %f\n" , SupervisorTemp);
	vSupervisor = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0x18],ORDER_LITTLEENDIAN); printf("vSupervisor = %f\n" , vSupervisor);
	vDetector = floatfrom((uint8_t*)&handle->mbuf.data[Offset+0x1C],ORDER_LITTLEENDIAN); printf("vDetector = %f\n" , vDetector);
}


void sendPollingSequence(sinclair_state *handle)
{
int32_t iretn;
char buf[4096];
uint16_t OpResultAddr;
uint16_t num_2_read;
uint32_t ResultLength;
//int16_t *DetTempADU;
//int16_t *ProcTempADU;
uint8_t command = 0x0B;
//struct timespec ts = {0,100L*1000000L};

int16_t DetTemp;
uint16_t ProcTemp;

uint32_t statusBits;
uint16_t darkOffsets[16];
uint8_t VddCoreSetPoint;
uint8_t MPUSetPoint;
//double AttQ[4];

iretn = nsp_stt_go(handle, buf, command);
printf("iretn=%d\nGo Command returns: %02X\n\n",iretn,buf[0]);

    for(int i=0 ; i<10 ; i++) {
		iretn = nsp_stt_read_edac_result_length(handle, buf);
        ResultLength = uint32from((uint8_t*)&buf[2], ORDER_LITTLEENDIAN);
        //printf("ResultLength = %d\n" , ResultLength);
        if(ResultLength==2616) {
            break;
        }
//        nanosleep(&ts, NULL);
		COSMOS_USLEEP(100000);
    }


    OpResultAddr= 0x48;
    num_2_read = 0x38;
	iretn = nsp_stt_read_result(handle, buf, OpResultAddr, num_2_read);     //0X0948 = 2376


    printf("Hardware telemetry\n-----------\n");

    // Decode and print detector temperature.
    DetTemp = int16from((uint8_t*)&buf[STT_DETTEMP_OFFSET],ORDER_LITTLEENDIAN);
    printf("DetTemp = %02X = %d dec --> DetTemp = %6.1f\n" , DetTemp, DetTemp, (double)(DetTemp>>4)/16.0);

    // Decode and print processor temperature.
    ProcTemp = uint16from((uint8_t*)&buf[STT_PROCTEMP_OFFSET],ORDER_LITTLEENDIAN);
    printf("ProcTemp = %02X = %d dec --> ProcTemp = %6.1f\n" , ProcTemp, ProcTemp, 25.0+(ProcTemp-0x32) * 1.5);



    // Get status bitfield.
    statusBits = uint32from((uint8_t*)&buf[STT_STATUS_BITFIELD],ORDER_LITTLEENDIAN);
    printf("Status Bitfield = 0x%08X\n" , statusBits);

    // Get Vdd core set point
    VddCoreSetPoint = buf[STT_VDDCORE_SETPOINT_OFFSET];
    printf("VddCoreSetPoint = 0x%02X\n" , VddCoreSetPoint);

    // Get MPU set point
    MPUSetPoint = buf[STT_MPU_SETPOINT_OFFSET];
    printf("MPUSetPoint = 0x%02X\n" , MPUSetPoint);

    printf("Reserved:%02X,%02X\n" , buf[0x16+70], buf[0x17]);

    // Print 16 dark offset values
    printf("darkOffsets:\n");
    for(int i=0 ; i<16 ; i++) {
        darkOffsets[i] = uint16from((uint8_t*)&buf[STT_DARKOFFSETS_OFFSET + i*2],ORDER_LITTLEENDIAN);
        printf("%d, " , darkOffsets[i]);
        if( (i==4)||(i==9) ) printf("\n");
    }
    printf("\n\n");
}



