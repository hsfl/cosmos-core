#include "rw_lib.h"
#include "cssl_lib.cpp"
#include <stdio.h>
#include <unistd.h>
#include <cstring>

int count;
int i,iretn;
FILE* fp;
char cmds[50][50];
char buf[128];

void setup();
int loop();

char arg1str[100] = "";
char arg2str[100] = "";
int arg2int = 0;


void parseCommandLine(int argc, char **argv )
{

//    int cmdLineParameterStringSize;
    char cmdLineParameter[100] = "";



    // right number of arguments
    if (argc == 3) {
        //puts(argv[1]);

        // its a global variable
        //hostPORT = atoi(argv[1]); //read port number from the command line

//        cmdLineParameterStringSize = strlen(argv[0]);
        strcpy (cmdLineParameter,argv[0]);

        strcpy (arg1str,argv[1]);
        strcpy (arg2str,argv[2]);

        puts(arg1str);
        puts(arg2str);

        arg2int = atoi(arg2str);

    } else {
        puts(">> ERROR: This programs needs two arguments to run. Ex: ./rw_test SETSPD 10");
        exit(0);

    }


}

/**
* doChecksum(char * string, int stringSize, char * chksum)
*	Calculates the checksum.
*	@return the checksum
**/
long int doChecksum(char * string) {
    int i;

    long int stringVal = 0;
    int stringSize = strlen(string);
    int asteriskPos = 0;
    for(i = 0; i < stringSize; i++) {
        if(string[i] == '*')
            asteriskPos = i;
    }
    for(i = 0; i < asteriskPos + 1; i++) {
        stringVal += string[i];
    }
    return stringVal;
}


int main(int argc, char* argv[]) {
    //fp = fopen("output.txt", "a+");

    parseCommandLine(argc,argv);

    setup();
    loop();
    return 0;
}

void setup() {
    i = rw_connect((char *)"/dev/ttyOS1");
    if (i != 0)
    {
        printf("Could not connect to reaction wheel in port (%s)", RW_DEVICE);
        printf("\n");
        exit(1);
    } else {
        printf("Sucessfully connected to %s\n",RW_DEVICE);
    }

    strcpy(cmds[0], "$IMU,GET,ALL*34A\n");
    strcpy(cmds[1], "$IMU,GET,YAW*362\n");
    strcpy(cmds[2], "$IMU,STREAM,START*4EB\n");
    strcpy(cmds[3], "$IMU,STREAM,STOP*4A3\n");
    strcpy(cmds[4], "$IMU,GET,ALL*34A\n");
    strcpy(cmds[5], "$IMU,GET,YAW*362\n");
    strcpy(cmds[6], "$IMU,STREAM,START*4EB\n");
    strcpy(cmds[7], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[8], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[9], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[10], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[11], "$SET,RW,SPD,0.0*3DC\n");
    strcpy(cmds[12], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[13], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[14], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[15], "$IMU,STREAM,START*4EB\n");
    strcpy(cmds[16], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[17], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[18], "$SET,RW,SPD,0.0*3DC\n");
    strcpy(cmds[19], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[20], "$SET,RW,SPD,-17.0*441\n");
    strcpy(cmds[21], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[22], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[23], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[24], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[25], "$SET,RW,ACC,10,SPD,16*535\n");
    strcpy(cmds[26], "$SET,RW,ACC,-10,SPD,0*52B\n");
    strcpy(cmds[27], "$SET,RW,SPD,0.0*3DC\n");
    strcpy(cmds[28], "$IMU,STREAM,START*4EB\n");
    strcpy(cmds[29], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[30], "$SET,RW,ACC,10.0*3ED\n");
    strcpy(cmds[31], "$SET,RW,SPD,0.0*3DC\n");
    strcpy(cmds[32], "$IMU,STREAM,STOP*4A3\n");

    printf("Table of commands: set\n");
}

int loop() {

    char cmdString[100] = "";
    uint32_t chksum = 0;

    if (strcmp (arg1str,"SETSPD") == 0) {

        sprintf(cmdString,"$SET,RW,SPD,%s*",arg2str);

        chksum = doChecksum(cmdString);
        printf("Checksum: %x\n",chksum); // return 441

        sprintf(cmdString,"$SET,RW,SPD,%s*%X\n",arg2str,chksum);
        printf("Setting speed of RW to %d\n",arg2int);

        //min -18.3 max 18.3

        rw_send(cmdString,1);
        printf("Command sucessfully sent: %s\n",cmdString);
    }
    
    if(strcmp(arg1str,"GET")==0)	{
    
    sprintf(cmdString,"$IMU,GET,%s*",arg2str);
    
	chksum = doChecksum(cmdString);
	printf("Checksum: %x\n",chksum); 
	
	sprintf(cmdString,"$IMU,GET,%s*%X\n",arg2str,chksum);
	printf("Getting %s from IMU\n",arg2str);
	
	rw_send(cmdString,1);
	printf("%s: ", arg2str);
	if(strcmp(arg2str,"ALL")==0)
		{
		rw_getdata(buf,128);
		printf("%s\n",buf);	
		}
	if(strcmp(arg2str,"YAW")==0)
		{
		rw_getdata(buf,128);
		printf("%s\n",buf);
		for(int i = 0; i < 128; i++) 
				{
            	buf[i] = '\0';
				}
		}
	}
	
	if(strcmp(arg1str,"STREAM")==0)	{
	
	sprintf(cmdString,"IMU,STREAM,%s*",arg2str);
    
	chksum = doChecksum(cmdString);
	printf("Checksum: %x\n",chksum); 
	
	sprintf(cmdString,"$IMU,STREAM,%s*%X\n",arg2str,chksum);
	printf("%sing data from IMU\n",arg2str);
	
	rw_send(cmdString,1);
	if(strcmp(arg2str,"START")==0)
		{
		while(1)
			{
			rw_getdata(buf,128);
			printf("%s\n",buf);
        	for(int i = 0; i < 128; i++) 
				{
            	buf[i] = '\0';
        		}
			}
		}
	if(strcmp(arg2str,"STOP")==0)		
		{
		rw_send(cmdString,1);
		printf("Command sucessfully sent: %s\n", cmdString);
		}
	}
		
    /*
    while(count < 28) {
        for(int i = 0; i < 128; i++) {
            buf[i] = '\0';
        }
        rw_send(cmds[count], 1);
        rw_getdata(buf, 128);
        printf("Issuing command: ");
        printf(cmds[count]);
        printf("\n");
        printf("Looking for data...");
        iretn = rw_getdata(buf, 128);
        if(iretn > 0) {
        	printf("Got output: ");
        	printf(buf);
        }
        if(iretn == RW_ERROR_FAILED) 
        	printf("Did not get anything within a second.");
        count++;
        sleep(3);
    }
    */
    return(0);
}
