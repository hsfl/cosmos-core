#include "support/configCosmos.h"
#include "support/timelib.h"

int main(int argc, char *argv[])
{
   calstruc date = mjd2cal(currentmjd());

   if (argc > 1)
   {
       date.year = atoi(argv[1]);
       if (argc > 2)
       {
           date.doy = atoi(argv[2]);
           if (argc > 3)
           {
               date.hour = atoi(argv[3]);
               if (argc > 4)
               {
                   date.minute = atoi(argv[4]);
                   if (argc > 5)
                   {
                       date.second = atoi(argv[5]);
                       if (argc > 6)
                       {
                           date.nsecond = atoi(argv[6]);
                       }
                   }
               }
           }
       }
   }

   double mjd = cal2mjd(date);
   printf("%.6f\n", mjd);

}
