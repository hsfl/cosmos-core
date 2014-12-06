
SUPPORT = ../../support
DEVICE = ../../device
LIBS = ../../support

CFLAGS = -g -pedantic -Wall -I. -I$(SUPPORT) -I$(DEVICE)

LFLAGS = -L$(LIBS) -lm -lpthread -L$(SUPPORT) -lCosmosSupport -L$(DEVICE) -lCosmosDevice


all : razor_imu


.c.o:
	gcc -c -std=c99 $(CFLAGS) $<

.cpp.o:
	g++ -c $(CFLAGS) $<

razor_imu.o : razor_imu.cpp $(DEVICE)/rw_lib.h $(DEVICE)/cssl_lib.h

razor_imu : razor_imu.o
	g++ $(CFLAGS) -o razor_imu razor_imu.o $(LFLAGS)

control_test1 : control_test1.o sinclair_lib.o microstrain_lib.o
	g++ $(CFLAGS) -o control_test1 control_test1.o sinclair_lib.o microstrain_lib.o $(LFLAGS)

control_test2.o : control_test2.c sinclair_lib.h microstrain_lib.h $(INCLUDES)/timelib.h $(INCLUDES)/mathlib.h

control_test2 : control_test2.o sinclair_lib.o microstrain_lib.o
	g++ $(CFLAGS) -o control_test2 control_test2.o sinclair_lib.o microstrain_lib.o $(LFLAGS)

control_test1.o : control_test1.c sinclair_lib.h microstrain_lib.h $(INCLUDES)/timelib.h $(INCLUDES)/mathlib.h

math_test.o : math_test.c $(INCLUDES)/mathlib.h

math_test : math_test.o
	g++ $(CFLAGS) -o math_test math_test.o $(LFLAGS)

microstrain_test.o : microstrain_test.c microstrain_lib.h

microstrain_test : microstrain_test.o microstrain_lib.o
	g++ $(CFLAGS) -o microstrain_test microstrain_test.o microstrain_lib.o $(LFLAGS)

microstrain_test2.o : microstrain_test2.c microstrain_lib.h

microstrain_test2 : microstrain_test2.o microstrain_lib.o
	g++ $(CFLAGS) -o microstrain_test2 microstrain_test2.o microstrain_lib.o $(LFLAGS)

microstrain_test3.o : microstrain_test3.c microstrain_lib.h

microstrain_test3 : microstrain_test3.o microstrain_lib.o
	g++ $(CFLAGS) -o microstrain_test3 microstrain_test3.o microstrain_lib.o $(LFLAGS)

microstrain_test4.o : microstrain_test4.c microstrain_lib.h

microstrain_test4 : microstrain_test4.o microstrain_lib.o
	g++ $(CFLAGS) -o microstrain_test4 microstrain_test4.o microstrain_lib.o $(LFLAGS)

microstrain_lib.o : microstrain_lib.cpp microstrain_lib.h $(INCLUDES)/mathlib.h cssl_lib.h

imu_test : imu_test.o rw_lib.o
	g++ $(CFLAGS) -o imu_test imu_test.o rw_lib.o $(LFLAGS)

rw_test : rw_test.o rw_lib.o
	g++ $(CFLAGS) -o rw_test rw_test.o rw_lib.o $(LFLAGS)

rw_test.o : rw_test.cpp rw_lib.h 

rw_test2 : rw_test2.o rw_lib.o
	g++ $(CFLAGS) -o rw_test2 rw_test2.o rw_lib.o $(LFLAGS)
	
rw_test2.o : rw_test2.cpp rw_lib.h

rw_lib.o : rw_lib.cpp rw_lib.h $(INCLUDES)/mathlib.h $(INCLUDES)/cosmos-errno.h cssl_lib.h

sinclair_test2.o : sinclair_test2.c sinclair_lib.h $(INCLUDES)/timelib.h

sinclair_test2 : sinclair_test2.o sinclair_lib.o
	g++ $(CFLAGS) -o sinclair_test2 sinclair_test2.o sinclair_lib.o $(LFLAGS)

sinclair_test.o : sinclair_test.c sinclair_lib.h

sinclair_test : sinclair_test.o sinclair_lib.o
	g++ $(CFLAGS) -o sinclair_test sinclair_test.o sinclair_lib.o $(LFLAGS)

sinclair_lib.o : sinclair_lib.cpp sinclair_lib.h cssl_lib.h

vmt35_test.o : vmt35_test.c vmt35_lib.h

vmt35_test : vmt35_test.o vmt35_lib.o
	g++ $(CFLAGS) -o vmt35_test vmt35_test.o vmt35_lib.o $(LFLAGS)

vmt35_test2.o : vmt35_test2.c vmt35_lib.h

vmt35_test2 : vmt35_test2.o vmt35_lib.o
	g++ $(CFLAGS) -o vmt35_test2 vmt35_test2.o vmt35_lib.o $(LFLAGS)

vmt35_lib.o : vmt35_lib.cpp vmt35_lib.h $(INCLUDES)/mathlib.h cssl_lib.h

cssl_lib.o : cssl_lib.cpp cssl_lib.h

libCosmosDevice.a : gs232b_lib.o  microstrain_lib.o  sinclair_lib.o ts2000_lib.o vmt35_lib.o cssl_lib.o rw_lib.o
	ar rs $@ $^
