COSMOSHOME = ../..
SUPPORT = $(COSMOSHOME)/support
DEVICE = $(COSMOSHOME)/device

OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
EXECUTABLES = $(patsubst %.cpp,%,$(wildcard *.cpp))
SUPPORT_LIB = $(SUPPORT)/libCosmosSupport.a
SUPPORT_INC = $(wildcard $(SUPPORT)/*.h)
SUPPORT_CPP = $(wildcard $(SUPPORT)/*.cpp)
DEVICE_LIB = $(DEVICE)/libCosmosDevice.a
DEVICE_INC = $(wildcard $(DEVICE)/*.h)
DEVICE_CPP = $(wildcard $(DEVICE)/*.cpp)

CFLAGS = -g -pedantic -Wall -I$(SUPPORT) -I$(DEVICE)

LFLAGS = -L$(SUPPORT) -L$(DEVICE) -lCosmosSupport -lCosmosDevice -lpthread -lm

%.o : %.c
	gcc -c $(CFLAGS) $<

%.o : %.cpp $(SUPPORT_INC) $(DEVICE_INC)
	g++ -c $(CFLAGS) $<

% : %.cpp $(SUPPORT_LIB) $(DEVICE_LIB)
	g++ -c $(CFLAGS) $<
	g++  $(CFLAGS) -o $@ $@.o $(LFLAGS)

% : %.o $(SUPPORT_LIB) $(DEVICE_LIB)
	g++  $(CFLAGS) -o $@ $@.o $(LFLAGS)

all : $(EXECUTABLES)

$(DEVICE_LIB) : $(DEVICE_CPP) $(DEVICE_INC) $(SUPPORT_CPP) $(SUPPORT_INC)
	$(MAKE) -C $(DEVICE)

$(SUPPORT_LIB) : $(SUPPORT_CPP) $(SUPPORT_INC)
	$(MAKE) -C $(SUPPORT)

clean: 
	rm -f *.o *.a $(EXECUTABLES)
