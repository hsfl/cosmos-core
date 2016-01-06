# SET THE COSMOS SOFTWARE HOME FOLDER
COSMOSHOME = ../../..
SUPPORT = $(COSMOSHOME)/support
DEVICE = $(COSMOSHOME)/device
TESTBED = $(COSMOSHOME)/testbed

EXECUTABLES= vmt35_test vmt35_stop vmt35_test_enable vmt35_tcm

CFLAGS = -pg -g -pedantic -Wall -I$(SUPPORT) -I$(DEVICE)
LFLAGS = -L$(SUPPORT) -L$(DEVICE) -lCosmosDevice -lCosmosSupport -lpthread -lm

.c.o:
	gcc -c -std=c99 $(CFLAGS) $<

.cpp.o:
	g++ -c $(CFLAGS) $<

all : depend $(EXECUTABLES)

depend :
	$(MAKE) -C $(DEVICE)
	$(MAKE) -C $(SUPPORT)

vmt35_test : vmt35_test.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o vmt35_test vmt35_test.o $(LFLAGS)

vmt35_test.o : vmt35_test.cpp

vmt35_stop : vmt35_stop.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o vmt35_stop vmt35_stop.o $(LFLAGS)

vmt35_stop.o : vmt35_stop.cpp

vmt35_test_enable : vmt35_test_enable.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o vmt35_test_enable vmt35_test_enable.o $(LFLAGS)

vmt35_test_enable.o : vmt35_test_enable.cpp

vmt35_tcm : vmt35_tcm.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o vmt35_tcm vmt35_tcm.o $(LFLAGS)

vmt35_tcm.o : vmt35_tcm.cpp

# CLEAN ALL THE PRODUCED FILES
clean :
	rm -Rf *.o *.user gmon.out $(EXECUTABLES)
