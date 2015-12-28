# Generic device {#tutorial-namespace-generic-device}

The COSMOS namespace provides access to a large number of commonly used devices. For each of these devices, as for example the GPS \ref gpsstruc, items are defined in an internal structure that represent the most commonly used values for this device. These internal values are then mapped to names in the COSMOS Namespace.

If some device is not in the COSMOS namespace then it can be added by extending the code and recompiling. However, one of the driving principles of COSMOS is to provide a generic environment that avoids the necessity of changing and/or recompiling the code to support something new. In the case where changes to the code are not desireable, or the amount of new information does not warrant changes, a \ref telemstruc has been provided. This generic telemetry device allows the assignment of a single value of any type, which then can be accessed as the appropriate type through standard names in the namespace. For further flexibility, these internally defined names can be mapped to user friendly names through \ref tutorial-namespace-aliases.

The following tutorial shows how to use a generic device so that new information can be added witout much effort. Another approach would be to create or add the piece of information directly into the namespace. This approach, although encouraged, requires more work. Please see \ref tutorial-namespace-add-new-device. 

The following example shows a typical use of generic devices. There is a self contained example that can be compiled and run using Qt creator from the folder <COSMOS>/core/examples/namespace/generic-device. The documenation of the example file can be found in agent_generic_device_neighbour.cpp and agent_generic_device_test.cpp
