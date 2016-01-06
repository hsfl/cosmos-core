# this .pro will compile all the cosmos-core programs that are also configured
# as .pri

TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

SUBDIRS += tutorials
