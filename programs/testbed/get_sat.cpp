#include "physicslib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

cosmosstruc *cdata;

int main(int argc, char *argv[])
{

cdata = json_create();
json_setup_node(argv[1],cdata);

create_databases(cdata);

}
