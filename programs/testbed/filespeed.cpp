#include "configCosmos.h"
#include "elapsedtime.hpp"
#include <errno.h>
// #include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/time.h>

char buffer[35000000];
struct timeval tp;
int32_t size;
int32_t start_s, start_u, diff_u;
int32_t i, j;
int32_t inb;
struct stat sbuf;
off_t tsize;

int main(int argc, char *argv[])
{
	int32_t iw, ir;
	int32_t werr, rerr;

	for (i=0;i<35000000;i++)
		buffer[i] = i%256;

	size = 1;
	for (i=0; i<6; i++)
	{
#if defined(COSMOS_WIN_OS)
		inb = open("testfile",O_RDWR|O_CREAT,00664);
#else
		inb = open("testfile",O_SYNC|O_RDWR|O_CREAT,00664);
#endif
		ElapsedTime et;
		et.start();
		//	gettimeofday(&tp,NULL);
		//	start_s = tp.tv_sec;
		//	start_u = tp.tv_usec;
		j=werr=rerr=0;
		do
		{
			iw = write(inb,buffer,size);
			if (iw != size)
			{
				printf("Write error: %d %d\n",iw,errno);
				werr++;
			}
			//		gettimeofday(&tp,NULL);
			//		diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
			j++;
			//		} while (diff_u<10000000L);
		} while (et.split() < 10.);
		close(inb);
		printf("Blocks: %7d x %7d ",j,size);
		printf("Write: %6.3f MB/sec (%d err) ",(size*j*1.)/diff_u,werr);
		fflush(stdout);
		inb = open("testfile",O_RDWR,00664);
		fstat(inb,&sbuf);
		//	gettimeofday(&tp,NULL);
		//	start_s = tp.tv_sec;
		//	start_u = tp.tv_usec;
		et.reset();
		j = 0;
		tsize = sbuf.st_size;
		do
		{
			ir = read(inb,buffer,size);
			if (ir != size)
				rerr++;
			//		gettimeofday(&tp,NULL);
			//		diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
			j++;
			if ((tsize-=size)<size)
			{
				lseek(inb,0,SEEK_SET);
				tsize = sbuf.st_size;
			}
			//		} while (diff_u<10000000L);
		} while (et.split() < 10.);
		close(inb);
		printf("Read: %6.3f MB/sec (%d err) \n",(size*j*1.)/diff_u, rerr);
		fflush(stdout);
		size *= 32;
	}
}
