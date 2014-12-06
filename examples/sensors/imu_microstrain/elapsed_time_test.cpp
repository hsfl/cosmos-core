#include <stdio.h>
#include <sys/time.h>
#include "timelib.h"

using namespace std;

int main()
{
struct timeval first, last, delta;
struct timezone x;

int num=rand();
int flag=0;

float timeDiff;

printf("Calculating...\n");

//Get the initial time
gettimeofday(&first, &x);

//Do the process
for(int i;i<1000000;i++)
{
    if(num==i)
    {
        printf("Found: %d ", num);
        flag=1;
    }
}

if(flag==0)
{
    printf("No results found ");
}

//Get the final time
gettimeofday(&last, &x);

//Caculate the difference
timeDiff=elapsed_time(delta,first,last);
printf("in %f milliseconds\n", timeDiff); 
}
