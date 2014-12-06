#include<stdio.h>
#include<stdlib.h>
 #include <unistd.h>
 
int main()
{
    printf("This c program will exit in 10 seconds.\n");        
 
    //delay(1000);    
    usleep(1000000);                     
 
    return 0;
}