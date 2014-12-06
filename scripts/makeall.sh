#!/bin/bash
src="../"

for i in $(find $src -name makefile) 
do
    echo $i
    make -f $i 
done
