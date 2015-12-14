CFLAGS = -pg -g -pedantic -Wall

all : testprofiler

.c.o:
	gcc -c -std=c99 $(CFLAGS) $<

testprofiler.o : testprofiler.c

testprofiler : testprofiler.o 
	g++ $(CFLAGS) -o testprofiler testprofiler.o

clean : 
	rm -f *.o testprofiler	

