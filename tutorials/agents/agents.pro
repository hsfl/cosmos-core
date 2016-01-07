# this .pro will compile all the cosmos-core programs that are also configured
# as .pri

TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

SUBDIRS += agent_001
SUBDIRS += agent_002
SUBDIRS += agent_add_soh
SUBDIRS += agent_calc
SUBDIRS += agent_cpu_device_test
SUBDIRS += agent_talkfree/agent_receive.pro
SUBDIRS += agent_talkfree/agent_send.pro

