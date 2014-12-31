#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

void test(){
    int i=0;
    printf("HAAAAAA!\n");
    for (i=0; i<1e4; i++) {
        printf("I love number: %d\n",i);
    }
}

int main()
{
    int i=0;

    printf("This c program will exit in 10 seconds.\n");        
 
    //delay(1000);    
    usleep(1000000);
    test();

    for (i=0; i<1e6; i++) {
        printf("I love number: %d\n",i);
    }
 
    return 0;
}
