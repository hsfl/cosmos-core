COSMOS Aliases
==============

purpose of COSMOS aliases: 



where is the code?
inside core/libraries/support/jsonlib

use the overloaded function 
json_addentry(string alias, string value, cosmosstruc *cdata)


thjings you can do

give an alias to an equation on the namespace

example: convert celclius to kelvin (this actually could be hadled differently by unit conversion)

the base unit in COSMOS for temp is K so le't sconvert to celcuis
C = K - 273.15

lets assume there is a namespace variable of imu tempperature "device_imu_temp_000"
then we can add equation to namespace
this is the json string that needs to be loaded
(\"device_imu_temp_000\" - 273.15)


to simplify the use of this equation we use an alias such as "imu_temp_celcius"