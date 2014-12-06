#include <stdio.h>
#include <unistd.h>
#include "vmt35_lib.h"
#include "microstrain_lib.h"

char arg1str[100] = "";
char arg2str[100] = "";
char arg3str[100] = "";
float arg2num = 0;
float arg3num = 0;

void getVoltageDac(int channel);
void parseCommandLine(int argc, char **argv );
void reset();

float miliamps = 0;

int main(int argc, char *argv[])
{

    int iretn; //, handle;
    //int16_t icurrent;
    //uint16_t ivoltage;
    float current;
    int channel;
    uint16_t voltagedac,voltagedacrx;
    //rvector magfield, mtrvec;
    //vmt35_telemetry telem;
    //int volts = 0;


    parseCommandLine(argc,argv);

//    iretn = vmt35_connect((char *)"dev_vmt35");
    iretn = vmt35_connect((char *)"/dev/ttyUSB4");
    //0403:e4ea

    if (iretn < 0)
    {
        printf("Error: vmt35_connect() %d\n",iretn);
        exit (iretn);
    }


    //A2 01 00 00 5C


    //printf(">> Get ready ...\n");
    //sleep(2);



    if (strcmp (arg1str,"RESET") == 0) {
        reset();
    }



    // set Set Torquer Supply Voltage is not really working
    // sets input voltage (29V) from system to Torque Rod
    // as long as this command is activated!!!
    //vmt35_set_percent_voltage(0,0);
    //vmt35_set_percent_voltage(1,0);
    //vmt35_set_percent_voltage(2,0);
    //vmt35_set_percent_voltage(2,25);
    //vmt35_set_percent_voltage(2,50);


    if (strcmp (arg1str,"SETVOLTAGE") == 0) {
        vmt35_set_percent_voltage(0,99);
        vmt35_set_percent_voltage(1,99);
        vmt35_set_percent_voltage(2,99);
    }




    //vmt35_set_percent_voltage(0,100);
    //vmt35_set_percent_voltage(1,100);

    if (strcmp (arg1str,"RUN") == 0) {

        current = arg2num ;
        printf(">> Enabling Supply Voltage ... ");
        if (vmt35_enable()==0)
            printf("Done\n");

        //sleep(1);

        vmt35_set_percent_voltage(0,99);
        vmt35_set_percent_voltage(1,99);
        vmt35_set_percent_voltage(2,99);

        printf(">> Setting Supply Current to Ch 0, %f A ... ",current);
        vmt35_set_amps(0,current);
        printf("Done\n");

        //sleep(1);

        printf(">> Setting Supply Current to Ch 1, %f A ... ",current);
        vmt35_set_amps(1,current);
        printf("Done\n");

        //sleep(1);

        printf(">> Setting Supply Current to Ch 2, %f A ... ",current);
        vmt35_set_amps(2,current);
        printf("Done\n");

        //sleep(5);


    }


    if (strcmp (arg1str,"SETCH") == 0) {

        channel = arg2num;
        current = ceilf(arg3num * 1000000) / 1000000;


        printf(">> Enabling Supply Voltage ... ");
        if (vmt35_enable()==0)
            printf("Done\n");

        //sleep(1);

        vmt35_set_percent_voltage(channel,99);

        printf(">> Setting Supply Current to Ch %d, %f A ... ",(int)channel, current);
        vmt35_set_amps(channel,current);
        //vmt35_set_miliamps(channel,miliamps);
        printf("Done\n");

    }

    if (strcmp (arg1str,"SETCDAC") == 0) {

        channel = arg2num;
        current = (uint16_t)arg3num;

        printf(">> Enabling Supply Voltage ... ");
        if (vmt35_enable()==0)
            printf("Done\n");

        //sleep(1);

        vmt35_set_percent_voltage(channel,99);

        printf(">> Setting Supply Current to Ch %d, %d DAC ... ",(int)channel, (int)current);
        vmt35_set_current_dac(channel,current);
        //vmt35_set_miliamps(channel,miliamps);
        printf("Done\n");

    }


    if (strcmp (arg1str,"GETV") == 0) {

        channel = arg2num;
        getVoltageDac(channel);


    }

    if (strcmp (arg1str,"GETAMP") == 0) {

        int16_t amps = 0;

        channel = arg2num;
        vmt35_get_current(channel,&amps);
        current = amps;
        printf(">> Got current: %d (raw) %f (A)\n",amps,current);


    }


    if (strcmp (arg1str,"SETVDAC") == 0) {

        voltagedacrx = 0;
        channel = arg2num;
        voltagedac = (uint16_t)arg3num;

        printf(">> Enabling Supply Voltage ... ");
        if (vmt35_enable()==0)
            printf("Done\n");

        printf(">> Setting Supply Voltage to Ch %d, %d DAC ... ",(int)channel, (int)voltagedac);
        //vmt35_set_current_dac(channel,current);
        //vmt35_set_miliamps(channel,miliamps);
        vmt35_set_voltage(channel,voltagedac);
        printf("Done\n");

        vmt35_get_voltage(channel,&voltagedacrx);
        printf(">> Voltage from DAC: %d\n",voltagedacrx);



    }


    if (strcmp (arg1str,"SETAMP") == 0) {
        printf(">> Setting Supply Current ... ");
        current = arg3num;
        vmt35_set_amps((int)arg2num,current);
        printf("Done\n");

    }

    if (strcmp (arg1str,"DISABLE") == 0) {
        printf(">> Disabling Supply Voltage ... ");
        vmt35_disable();
        printf("Done\n");
    }

    if (strcmp (arg1str,"ENABLE") == 0) {
        printf(">> Enabling Supply Voltage ... ");
        if (vmt35_enable()==0)
            printf("Done\n");
    }

exit (0);
    while (0)
    {
        vmt35_set_amps(0,current);
        vmt35_set_amps(1,current);
        vmt35_set_amps(2,current);



        /*
    vmt35_get_telemetry(&telem);
    printf("%hu %hu %hu %d %d %d %hu %hu\n",telem.status,telem.count,telem.invalidcount,telem.daca,telem.dacb,telem.dacc,telem.temp,telem.voltage);

    sleep(2);
    vmt35_disable();
*/
        /*
    current = 0.;
    vmt35_set_amps(0,current);
    vmt35_set_amps(1,current);
    vmt35_set_amps(2,current);
    */
        //    vmt35_get_telemetry(&telem);
        //	printf("%hu %hu %hu %d %d %d %hu %hu\n",telem.status,telem.count,telem.invalidcount,telem.daca,telem.dacb,telem.dacc,telem.temp,telem.voltage);

    }

    //vmt35_get_current(0,&icurrent);

}


// reset
void reset(){
    printf(">> Reseting ... ");
    if (vmt35_reset()==0)
        printf("Done!\n");

    sleep(2);
}

void getVoltageDac(int channel){
    uint16_t voltagedacrx = 0;
    float volts = 0;

    vmt35_get_voltage(channel,&voltagedacrx);
    volts = voltagedacrx/4095.0*29.0;
    printf(">> Voltage from DAC: %d, %f V\n",voltagedacrx,volts);
}


void parseCommandLine(int argc, char **argv )
{

    int  cmdLineParameterStringSize; //argi,
    char cmdLineParameter[100] = "";



    switch ( argc ) {
    //case 1:            // Note the colon, not a semicolon
    //playgame();
    //  break;
    case 2:            // Note the colon, not a semicolon
        //loadgame();
        strcpy (arg1str,argv[1]);
        break;
    case 3:            // Note the colon, not a semicolon

        // ./vmt35_test_enable RUN 0.1
        strcpy (arg1str,argv[1]);
        strcpy (arg2str,argv[2]);
        arg2num = atof(arg2str);
        break;
    case 4:            // Note the colon, not a semicolon
        //puts(argv[1]);

        // ./vmt35_test_enable SETCH 0 0.1

        // its a global variable
        //hostPORT = atoi(argv[1]); //read port number from the command line

        cmdLineParameterStringSize = strlen(argv[0]);
        strcpy (cmdLineParameter,argv[0]);

        strcpy (arg1str,argv[1]);
        strcpy (arg2str,argv[2]);
        strcpy (arg3str,argv[3]);

        //puts(arg1str);
        //puts(arg2str);

        arg2num = atof(arg2str);
        arg3num = atof(arg3str);
        break;
    default:            // Note the colon, not a semicolon
        //cout<<"Error, bad input, quitting\n";
        puts(">> ERROR: This programs needs two arguments to run. Ex: ./vmt35_test_enable SETCH 0 0.1");
        exit(0);

        break;
    }

}

