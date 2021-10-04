#!/bin/tcsh
foreach day ( `seq $1 $2` )
   wget -O bulletinb-$day.txt https://datacenter.iers.org/data/csv/bulletinb-$day.csv
end
