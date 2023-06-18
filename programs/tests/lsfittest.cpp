#include "support/configCosmos.h"
#include "math/mathlib.h"

int main(int argc, char **)
{
    LsFit tfit(9,8);

    for (size_t i=0; i<100; ++i)
    {
        double nextx = i * RADOF(20.);
        rvector nexty;
        nexty.col[0] = cos(nextx);
        nexty.col[1] = cos(nextx+RADOF(10.));
        nexty.col[2] = cos(nextx+RADOF(60.));
        tfit.update(nextx, nexty);
        for (size_t j=0; j<5; ++j)
        {
            nextx += RADOF(4.);
            rvector currenty = tfit.evalrvector(nextx);
            printf("%f\t%f\t%f\t%f\n", nextx, currenty.col[0]-cos(nextx), currenty.col[1]-cos(nextx+RADOF(10.)), currenty.col[2]-cos(nextx+RADOF(60.)));
        }
    }
}
