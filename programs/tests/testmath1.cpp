#include "math/mathlib.h"
#include "math/vector.h"

int main(int argc, const char* argv[])
{
    int i;
    double aarray [3];
    double barray [3];
    double sarray [3];
    rvector a,b,sol,actualOutput;
    if(argc != 10)
    {
        printf("%d arguments is not allowed 10 are required", argc);
        return 1;
    }else
    {
        for(i= 1; i < 4; i++)//Load rvectors
        {
            aarray[i-1] = atof(argv[i]);
            barray[i-1] = atof(argv[i+3]);
            sarray[i-1] = atof(argv[i+6]);
        }
        printf("%f %f %f\n", aarray[0],aarray[1],aarray[2]);
        printf("%f %f %f\n", barray[0],barray[1],barray[2]);
        printf("%f %f %f\n", sarray[0],sarray[1],sarray[2]);
        a = {aarray[0],aarray[1],aarray[2]};
        b = {barray[0],barray[1],barray[2]};
        sol = {sarray[0],sarray[1],sarray[2]};
        actualOutput = rv_sub(a,b);//Get function output
        if (actualOutput == sol)//check if the sol entered matches function output
        {
            printf("rv_sub passed\n");
            return 0;
        }else
        {
            printf("rv_sub failed\n");
            return 1;
        }
    }
}
