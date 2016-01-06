# SET THE COSMOS SOFTWARE HOME FOLDER
COSMOSHOME = ../../..
SUPPORT = $(COSMOSHOME)/support
DEVICE = $(COSMOSHOME)/device
TESTBED = $(COSMOSHOME)/testbed

EXECUTABLES= rw_test

CFLAGS = -pg -g -pedantic -Wall -I$(SUPPORT) -I$(DEVICE)
LFLAGS = -L$(SUPPORT) -L$(DEVICE) -lm -lpthread -lCosmosSupport -lCosmosDevice

.c.o:
	gcc -c -std=c99 $(CFLAGS) $<

.cpp.o:
	g++ -c $(CFLAGS) $<

#all: $(MAKE) -C $(DEVICE)/makefile

rw_test : rw_test.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o rw_test rw_test.o $(LFLAGS)

rw_test.o : rw_test.cpp

# CLEAN ALL THE PRODUCED FILES
clean :
	rm -f *.o $(EXECUTABLES)
