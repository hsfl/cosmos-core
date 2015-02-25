#include "configCosmos.h"
#include <stdio.h>
#include <math.h>
#include "mathlib.h"

#define HEIGHT 1.05
#define DEPTH 1.5
#define IRADIUS .8
#define ORADIUS 2.5

int main(int argc, char *argv[])
{
double sum1, sum2, sum3, sum4, az, el, size;
double dsize, xcinner, ycinner, rinner, parea, radius;
double x, y, newx, newy, sel, xcouter, ycouter, router;
int i, j, k, m, counter1, counter2, counter3, counter4;

size = ORADIUS+2.*IRADIUS;
dsize = size / 300.;
parea = dsize * dsize / (M_PI*IRADIUS*IRADIUS);

el = DPI2;
for (i=0; i<40; i++)
	{
	if (el == DPI2)
		rinner = router = 0.;
	else
		{
		rinner = DEPTH / tan(el);
		router = (DEPTH+HEIGHT) / tan(el);
		}
	az = 0.;
	sel = sin(el);
	for (j=0; j<40; j++)
		{
		xcinner = rinner * sin(az);
		ycinner = rinner * cos(az);
		xcouter = router * sin(az);
		ycouter = router * cos(az);
		sum1 = sum2 = sum3 = sum4 = 0.;
		counter1 = counter2 = counter3 = counter4 = 0;
		y = -size;
		for (k=-300;k<=300;k++)
			{
			x = -size;
			for (m=-300;m<=300;m++)
				{
				if (k != 0 && m != 0)
					{
					radius = sqrt(x*x+y*y);
					if (radius > ORADIUS*.8)
						{
						counter1++;
						}
					else
						{
						radius = sqrt((x-xcinner)*(x-xcinner)+(y-ycinner)*(y-ycinner));
						if (radius > IRADIUS)
							{
							counter2++;
							}
						else
							{
							radius = sqrt((x-xcouter)*(x-xcouter)+(y-ycouter)*(y-ycouter));
							if (radius > ORADIUS)
								{
								counter3++;
								}
							else
						{
						counter4++;
						if (x<0.)
							{
							if (y<0.)
								{
								sum3 += sel*parea;
								}
							else
								{
								sum1 += sel*parea;
								}
							}
						else
							{
							if (y<0.)
								{
								sum4 += sel*parea;
								}
							else
								{
								sum2 += sel*parea;
								}
							}
						}
						}
						}
					}
				x+=dsize;
				}
			y+=dsize;
			}
		newx = (sum2+sum4-sum1-sum3);
		newy = (sum1+sum2-sum3-sum4);
		printf("%f %f %f %f %f %f %f %f %f %f %f %d %d %d %d\n",el,az,xcinner,ycinner,sum1,sum2,sum3,sum4,newx,newy,sqrt(newx*newx+newy*newy),counter1,counter2,counter3,counter4);
		az += D2PI/40.;
		}
	el -= DPI2/40.;
	}
}
