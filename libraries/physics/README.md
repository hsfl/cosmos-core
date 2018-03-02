Physics Library {#library-physics}
===============

A library containing functions for simulating the orbital
environment of a spacecraft.

The \ref physicslib_functions in this library allow you to work with the various
aspects of a spacecrafts environment including:
- Position: The physical location of the spacecraft wrt various
frames of reference.
- Attitude: The orientation of the spacecraft in various frames of
reference.
- Power: Power produced and used by the spacecraft.
- Temperature: Thermal inputs, outputs and flows, and their effect
on overall temperature.
- Forces: Linear and angular forces exerted both externally and
internally.
- Hardware State: Aspects of the internal state of various
subsystems.

The goal of this library is to provide for as full a simulation as
possible of the complete spacecraft. The \ref physicslib_typedefs are used to define the structure of the spacecraft.
Both Static and Dynamic definitions exist for element. The elements
are described using the following categories:
- Structures: Physical elements of the spacecraft. These are
located in the Body frame of the spacecraft and can be:
        - Panel: 4 corners and a thickness
        - Box: 6 sides and a wall thickness
        - Cylinder: 2 ends and a radius
        - Sphere: center and a radius
- Components: Logical elements of the spacecraft. They represent
additional physical elements of the spacecraft that are not purely
structural. They have physical qualities (mass, power, temperature)
tied to them, and are in turn tied to a Structure.
- Devices: Specialized Component extensions. Each different type has
specialized information tied to it, as well as being tied to a
Component. Currently supported Devices are:
        - Reaction Wheel
        - IMU
        - GPS
        - Processor (CPU)
        - Magnetic Torque Rod (MTR)
        - Sun Sensor (SS)
        - Solar Panel String
-
Many of the elements used in the ::cosmosstruc are defined here.

