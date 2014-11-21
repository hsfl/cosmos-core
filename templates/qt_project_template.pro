# *** ADD PROJECT DESCRIPTION
# TEMPLATE FOR QT CONSOLE PROJECTS

# *************************************
# QT Config
TEMPLATE    = app
CONFIG      += console                      # for terminal application
CONFIG      -= qt                           # (-) will not add qt stuff, (+) will add qt stuff,
CONFIG      -= app_bundle
#TARGET      = my_program                   # *** to specify the program name  (or use $$PROGRAM_NAME)

# *************************************
# USER CONFIG
# PROGRAM_NAME = my_program                 # *** change this

# *************************************
# DEFINE COSMOS SOFTWARE FOLDERS
COSMOS_SW   = ..                           # *** you may need to change this
SUPPORT     = $$COSMOS_SW/support
DEVICE      = $$COSMOS_SW/device
THIRDPARTY  = $$COSMOS_SW/thirdparty

# *************************************
# to add more cpp files right click on the project file and "Add Existing Files"
SOURCES += \
    my_program.cpp                          # *** change this,

HEADERS += \
    my_program.h                            # *** change this

# *************************************
# if you want to add all COSMOS device
# and support files to this project
SOURCES += \
    $$files($$DEVICE/*.cpp) \               # *** you may want to uncomment this
    $$files($$SUPPORT/*.cpp)\               # *** you may want to uncomment this

HEADERS += \
    $$files($$DEVICE/*.h) \               # *** you may want to uncomment this
    $$files($$SUPPORT/*.h)\               # *** you may want to uncomment this

# *************************************
OBJECTS_DIR = $$OUT_PWD/obj

# *************************************
# use this to change the qmake-genarated makefile name
# this will avoid overwriting the default makefile
QMAKE_MAKEFILE  = makefileqt
MAKEFILE        = makefileqt

# *************************************
# remove the objects directory, the makefileqt and the executable
QMAKE_CLEAN += -r $$OBJECTS_DIR makefileqt $$TARGET

# *************************************
INCLUDEPATH += $$SUPPORT
INCLUDEPATH += $$DEVICE
INCLUDEPATH += $$THIRDPARTY

# *************************************
# compile fot c++0X
# mingw automatically adds -ansi flag so to be more flexible use -U__STRICT_ANSI__
# Changing -std=c++0x to -std=gnu++0x may also help to fix some issues
QMAKE_CXXFLAGS += -std=c++0x -U__STRICT_ANSI__

# *************************************
# for windows
win32 {
    message( "Building on Win32" )
}

# *************************************
# for linux
unix:!macx{ # or maybe linux-g++ {
    message( "Building on Linux" )
}

# *************************************
# for mac os x
macx {
    message( "Building on Mac X" )
}
