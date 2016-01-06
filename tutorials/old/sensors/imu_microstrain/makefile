COSMOSHOME = ../../..
SUPPORT = $(COSMOSHOME)/support
DEVICE = $(COSMOSHOME)/device
EXECUTABLES= microstrain_imu_simple measure_trods

CFLAGS = -pg -g -pedantic -Wall -I$(SUPPORT) -I$(DEVICE)

# must have CosmosDevice before CosmosSupport
LFLAGS = -L$(SUPPORT) -L$(DEVICE) -lm -lCosmosDevice -lCosmosSupport -lpthread

.c.o:
	gcc -c -std=c99 $(CFLAGS) $<

.cpp.o:
	g++ -c $(CFLAGS) $<

all : $(EXECUTABLES)

microstrain_imu_simple : microstrain_imu_simple.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o microstrain_imu_simple microstrain_imu_simple.o $(LFLAGS)

microstrain_imu_simple.o : microstrain_imu_simple.cpp

measure_trods : measure_trods.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o measure_trods measure_trods.o $(LFLAGS)

measure_trods.o : measure_trods.cpp

elapsed_time_test : elapsed_time_test.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o elapsed_time_test elapsed_time_test.o $(LFLAGS)

elapsed_time_test.o : elapsed_time_test.cpp

measure_trods : measure_trods_C.o $(SUPPORT)/libCosmosSupport.a $(DEVICE)/libCosmosDevice.a
	g++ $(CFLAGS) -o measure_trods_C measure_trods_C.o $(LFLAGS)

measure_trods.o : measure_trods_C.cpp

clean :
	rm -f *.o *.user makefileq* gmon* $(EXECUTABLES) microstrain_imu_simple
