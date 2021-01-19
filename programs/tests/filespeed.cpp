#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

char buffer[70000000];
struct timeval tp;
size_t size;
long start_s, start_u, diff_u;
long i, j;
int inb;
struct stat sbuf;
off_t tsize;

int main(int argc, char *argv[])
{
ssize_t iw, ir;
long werr, rerr;

for (i=0;i<70000000;i++)
	buffer[i] = i%256;

size = 32;
for (i=0; i<8; i++)
	{
	inb = open("testfile",O_SYNC|O_RDWR|O_CREAT,00664);
	gettimeofday(&tp,NULL);
	start_s = tp.tv_sec;
	start_u = tp.tv_usec;
	j=werr=rerr=0;
	do
		{
		iw = write(inb,buffer,size);
		if (iw != size)
			{
			printf("Write error: %lu %ld %d\n",size,iw,errno);
			werr++;
			}
		gettimeofday(&tp,NULL);
		diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
		j++;
		} while (diff_u<10000000L);
	close(inb);
	printf("Blocks: %7ld x %8lu ",j,size);
	printf("Write: %6.3lf MB/sec (%ld err) ",(size*j*1.)/diff_u,werr);
	fflush(stdout);
	inb = open("testfile",O_RDWR,00664);
	fstat(inb,&sbuf);
	gettimeofday(&tp,NULL);
	start_s = tp.tv_sec;
	start_u = tp.tv_usec;
	j = 0;
	tsize = sbuf.st_size;
	do
		{
		ir = read(inb,buffer,size);
		if (ir != size)
			rerr++;
		gettimeofday(&tp,NULL);
		diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
		j++;
		if ((tsize-=size)<size)
			{
			lseek(inb,0,SEEK_SET);
			tsize = sbuf.st_size;
			}
		} while (diff_u<10000000L);
	close(inb);
	printf("Read: %6.3lf MB/sec (%ld err) \n",(size*j*1.)/diff_u, rerr);
	fflush(stdout);
	size *= 8;
	}
}
