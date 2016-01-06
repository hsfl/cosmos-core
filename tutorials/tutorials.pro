# this .pro will compile all the cosmos-core programs that are also configured
# as .pri

TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

SUBDIRS += agents/agent_001
SUBDIRS += agents/agent_002
SUBDIRS += agents/agent_add_soh
SUBDIRS += agents/agent_calc
SUBDIRS += agents/agent_cpu_device_test
SUBDIRS += agents/agent_talkfree/agent_receive.pro
SUBDIRS += agents/agent_talkfree/agent_send.pro

# namespace tutorials
SUBDIRS += namespace/alias
SUBDIRS += namespace/generic-device/agent_generic_device_neighbour.pro
SUBDIRS += namespace/generic-device/agent_generic_device_test.pro

# physics
#SUBDIRS += physics/propagator/propagator_simple.pro
