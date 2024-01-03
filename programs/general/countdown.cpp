#include "support/cosmos-errno.h"
#include "support/timelib.h"

int main(int argc, char* argv[])
{
    string message = "Countdown";
    int32_t seconds = 10;
    int32_t steps = 1;

    switch (argc)
    {
    case 4:
        message = argv[3];
    case 3:
        steps = atoi(argv[2]);
    case 2:
        seconds = atoi(argv[1]);
        break;
    default:
        printf("Usage: countdown seconds [steps [message]]\n");
        exit (1);
    }

    timed_countdown(seconds, steps, message);
}
