#include "rw_lib.h"
#include "cssl_lib.cpp"
#include <stdio.h>
#include <unistd.h>

int count;
int i;
FILE* fp;
char cmds[15][50];
char buf[128];

void setup();
int loop();

int main(int argc, char* argv[]) {
	//fp = fopen("output.txt", "a+");
	setup();
	loop();
	return 0;
}

void setup() {

i = rw_connect("/dev/ttyUSB4");
if(i != 0) {
	printf("Could not connect to reaction wheel.");
	printf("\n");
	exit(1);
}

strcpy(cmds[0], "$IMU,GET,ALL*34A");
strcpy(cmds[1], "$IMU,GET,YAW*362");

strcpy(cmds[2], "$IMU,STREAM,START*4EB");
strcpy(cmds[3], "$IMU,STREAM,STOP*4A3");
strcpy(cmds[4], "$IMU,GET,ALL*34A");
strcpy(cmds[5], "$IMU,GET,YAW*362");
strcpy(cmds[6], "$IMU,STREAM,START*4EB");
strcpy(cmds[7], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[8], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[9], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[10], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[11], "$SET,RW,SPD,0.0*3DC");
strcpy(cmds[12], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[13], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[14], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[15], "$IMU,STREAM,START*4EB");
strcpy(cmds[16], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[17], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[18], "$SET,RW,SPD,0.0*3DC");
strcpy(cmds[19], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[20], "$SET,RW,SPD,-17.0*441");
strcpy(cmds[21], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[22], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[23], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[24], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[25], "$SET,RW,ACC,10,SPD,16*535");
strcpy(cmds[26], "$SET,RW,ACC,-10,SPD,0*52B");
strcpy(cmds[27], "$SET,RW,SPD,0.0*3DC");
strcpy(cmds[28], "$IMU,STREAM,START*4EB");
strcpy(cmds[29], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[30], "$SET,RW,ACC,10.0*3ED");
strcpy(cmds[31], "$SET,RW,SPD,0.0*3DC");
strcpy(cmds[32], "$IMU,STREAM,STOP*4A3");
}

int loop() {
	while(count < 33) {
		for(int i = 0; i < 128; i++) {
			buf[i] = '\0';
		}
		rw_send(cmds[count], 1);
		rw_getdata(buf, 128);
		printf("Command: ");
		printf(cmds[count]);
		printf("\n");
		printf("Output: ");
		printf(buf);
		count++;
	}
	return(0);
}
