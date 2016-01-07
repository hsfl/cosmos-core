# this .pro will compile all the cosmos-core libraries, tutorials and programs

TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11
CONFIG += ordered

message("--------------------")
message("Building cosmos-core")

SUBDIRS += libraries
SUBDIRS += tutorials
