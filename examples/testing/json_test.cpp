#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonlib.h"

cosmosstruc *cosmos_data;

int main(int argc, char *argv[])
{

double result;
json_create(&cosmos_data);
if(argc == 2) {
	const char *ptr = argv[1];
	result = json_equation(&ptr,cosmos_data);
	printf("%f \n", result);
}

else if(argc == 1) {
	double timestamp = 500.;
	double node_utc = 400.;
	char testString[128];
	const char *strPtr = &testString[0];
	const char testOperators[10] = {"+-*/&|><="};
	json_set_double_name(timestamp, (char *)"timestamp",cosmos_data);
	json_set_double_name(node_utc, (char *)"node_utc",cosmos_data);
	
	for(int i = 0; i < 9; i++) {
		strPtr = testString;
		strcpy(testString, "(\"timestamp\"");
		testString[strlen(testString)] = testOperators[i];
		strcat(testString, "\"node_utc\")");
		result = json_equation(&strPtr,cosmos_data);
		printf("%f \n", result);
		for(int j = 0; j < 128; j++) {
			testString[j] = 0;
		}
	}
	
}
else {
	printf("Too many arguments \n");
	return 0;
}
}
