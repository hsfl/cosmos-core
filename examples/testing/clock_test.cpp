/* clock example: frequency of primes */
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

int frequency_of_primes (int n) {
    int i,j;
    int freq=n-1;
    for (i=2; i<=n; ++i) for (j=(int)sqrt(i);j>1;--j) if (i%j==0) {--freq; break;}
    return freq;
}

int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

float elapsed_time(struct timeval dif, struct timeval a,struct timeval b)
{
    /*Subtract the timevals*/
    timeval_subtract(&dif,&a,&b);

    /* Calculate and return difference in milliseconds */
    return -dif.tv_sec*1000.0-dif.tv_usec/1000.0;
}

int main ()
{
    int f;
    int ti,tf,dt;
    printf ("Calculating...\n");

    struct timeval tv1,tv2,diff;
    struct timezone tz;
    struct tm *tm;

    float k;

    ti = clock();
    //setitimer(ITIMER_REAL,v, ov);
    gettimeofday(&tv1, &tz);
    f = frequency_of_primes (9000);
    gettimeofday(&tv2, &tz);
    //getitimer(ITIMER_REAL,v);
    tf = clock();

    tm=localtime(&tv1.tv_sec);
    printf(" %d:%02d:%02d %d \n", (int)tm->tm_hour, (int)tm->tm_min, (int)tm->tm_sec, (int)tv1.tv_usec);

    tm=localtime(&tv2.tv_sec);
    printf(" %d:%02d:%02d %d \n", (int)tm->tm_hour, (int)tm->tm_min, (int)tm->tm_sec, (int)tv2.tv_usec);

    //timeval_subtract(&diff, &tv1, &tv2);
    k = elapsed_time(diff, tv1, tv2);

    //printf("It took me %d microseconds...\n", -diff.tv_sec*1000000-diff.tv_usec);
    printf("It took me %f milliseconds\n", k);
    //printf("It took me %d microseconds... \n", v.it_value.tv_usec);


    dt = tf - ti;
    printf ("The number of primes lower than 100,000 is: %d\n",f);
    printf ("It took me %d clicks (%f seconds). CPS: %f\n",dt,dt/(double)CLOCKS_PER_SEC,(double)CLOCKS_PER_SEC);

    time_t seconds;

    time_t     now = time(0);
    //time_t  time;
    //time(&time)

    seconds = time (&now);
    printf ("%ld hours since January 1, 1970\n", seconds/3600);

    return 0;
}
