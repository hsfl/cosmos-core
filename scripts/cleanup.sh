#!/bin/bash

# go to every folder in the cosmos software and clean any existing binaries
src="../"

for i in $(find $src -name makefile) 
do
    echo $i
    make -f$i clean
done
