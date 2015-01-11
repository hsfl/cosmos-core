#include "physicslib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
int16_t n;
string output;
FILE *odes;
string jstring;

cdata = json_create();
load_databases(argv[1],(uint16_t)atol(argv[2]),cdata);

// Battery capacity
cdata->node.battcap = 0.;
for (n=0; n<cdata->devspec.batt_cnt; n++)
	{
	cdata->node.battcap += cdata->devspec.batt[n]->capacity;
	}
cdata->node.battlev = cdata->node.battcap;

output = json_of_node(jstring, cdata);
odes = fopen("node.ini","w");
fputs(output.c_str(),odes);
fclose(odes);
//output = json_groundstation(jstring, cdata);
//odes = fopen("groundstation.ini","w");
//fputs(output,odes);
//fclose(odes);
//create_ui_tcs();
}
