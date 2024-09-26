#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/convertdef.h"
#include "support/convertlib.h"
#include "support/jsondef.h"
#include <fstream>

using namespace json11;

int main(int argc, char *argv[])
{
	cout<<"Testing with cosmosstruc"<<endl;

	// BUT... if you comment out this declaration it builds...  why?
	Cosmos::Support::cosmosstruc c;
	
	return 0;

}
