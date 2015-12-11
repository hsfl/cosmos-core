# Aliases {#tutorial-namespace-aliases}

COSMOS aliases can be used to rename COSMOS namespace names or equations. The following example shows a typical use of COSMOS aliases. There is a self contained example that can be run using Qt creator from the folder <COSMOS>/core/examples/namespace/alias. The documenation of the example file can be found in this link: alias.cpp.

## Example
Objective: convert the IMU temperature data from degrees Celcius to Kelvin. This actually could be handled differently by unit conversion but the purpose here is to demonstrate the use of equations in COSMOS.

The base unit in COSMOS for temperatures is Kelvin so lets define the equation to convert to from Kelvin to Celcuis \f$ C = K - 273.15 \f$

The namespace variable for the imu tempperature in this case is "device_imu_temp_000", knowing this we can add equation to namespace. The equation will be formed as a JSON string 
```
(\"device_imu_temp_000\" - 273.15)
```
To simplify the use of this equation we use an alias such as 
```
"imuTempCelcius"
```

The code to handle COSMOS namespace aliases and equations can be found in jsonlib.cpp. The relevant functions for aliases are \ref json_addentry and \ref json_get_double. For equations the relevant functions are \ref json_equation and \ref json_equation_map.

