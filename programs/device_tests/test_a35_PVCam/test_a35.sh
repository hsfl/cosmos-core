#!/bin/bash

# Get the executable's absolute path
DIRNAME="/home/mark"

# set env. vars
export PUREGEV_ROOT=$DIRNAME
export GENICAM_ROOT=$DIRNAME/lib/genicam
export GENICAM_ROOT_V2_2=$GENICAM_ROOT


LD_LIBRARY_PATH=/home/mark/lib:/home/mark/lib/genicam/bin/Linux64_x64

export LD_LIBRARY_PATH

./test_a35
