# this .pro will compile all the cosmos-core programs that are also configured
# as .pri

TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11
CONFIG += ordered

LIBS += -L$$OUT_PWD

SUBDIRS += libraries

SUBDIRS += tutorials/agents/agent_001
SUBDIRS += tutorials/agents/agent_002

agent_001.depends = support math

#SUBDIRS += libraries/support
#SUBDIRS += libraries/math
#SUBDIRS += tutorials
