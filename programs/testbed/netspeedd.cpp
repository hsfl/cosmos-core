#include "configCosmos.h"
//#include <sys/types.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
// #include <unistd.h>

/*
void abort_func(int sig, int code, struct sigcontext *scp, char *addr);
int gethostname(char *name, int namelen);
int shutdown(int s, int how);
*/
void abort_func(int);

char command[50];
char inbuf[10000000];
struct timeb etime, btime;
double ttime, bps, kbps;
int32_t i, s, ns;

//extern int errno;

int main(int argc, char *argv[])
{
	//  char hostname[64];
	//  struct hostent *hp;
	socklen_t fromlen;
	int32_t number, size, inchar;
	uint16_t port;
	struct sockaddr_in sin, fsin;

	port = atol(argv[1]);
	//  gethostname(hostname, sizeof(hostname));

	//  if ((hp = gethostbyname(hostname)) == NULL)
	//     {
	//     fprintf(stderr, "%s: host unknown.\n", hostname);
	//     exit(1);
	//     }

#ifdef COSMOS_WIN_OS
//	unsigned long nonblocking = 1;
//	struct sockaddr_storage ss;
//	int sslen;
	WORD wVersionRequested;
	WSADATA wsaData;
	static bool started=false;

	if (!started)
	{
		wVersionRequested = MAKEWORD( 1, 1 );
		WSAStartup( wVersionRequested, &wsaData );
	}
#endif

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("server: socket");
		exit(1);
	}

	sin.sin_family = AF_INET;
	sin.sin_port = (unsigned short)htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind((int)s, (struct sockaddr *)&sin, sizeof(sin)) <0)
	{
		perror("server: bind");
		exit(1);
	}

	signal(SIGINT, abort_func);

	if (listen(s,5) < 0)
	{
		perror("server: listen");
		exit(1);
	}

	if ((ns = accept(s, (struct sockaddr *)&fsin, &fromlen)) < 0)
	{
		perror("server: accept");
		exit(1);
	}
do_listen:

	recv(ns, command, 50, 0);
	sscanf(command, "%d %d", &number, &size);
	ftime(&btime);
	printf("%d * %d = %d / ",number,size,number*size);
	fflush(stdout);
	for (i=0; i<number; i++)
	{
		inchar = 0;
		do
		{
			inchar += recv(ns, inbuf, size-inchar, 0);
		} while (inchar < size);
	}
	ftime(&etime);
	ttime = ((double)etime.time+(double)etime.millitm/1000.)-((double)btime.time+(double)btime.millitm/1000.);
	bps = (double)number*(double)size/ttime;
	kbps = bps / 1024.;
	printf("%7.3f = %7.1f\r",ttime,kbps);
	sprintf(command,"%f",kbps);
	send(ns, command, 50, 0);
	if (number)
		goto do_listen;

	shutdown(s, 2);
}

/*
void abort_func(int sig, int code, struct sigcontext *scp, char *addr)
*/
void abort_func(int)
{
	shutdown(ns, 2);
	shutdown(s, 2);
	exit(2);
}
