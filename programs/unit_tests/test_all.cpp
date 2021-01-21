#include "test_all.h"
#include "test_cosmosstruc.h"
#include "test_sph_sim.h"

int main(int argc, char** argv)	{

	cout<<"RUNNING ALL UNIT TESTS..."<<endl;

	run_all_tests(argc, argv);

	return 0;
}
