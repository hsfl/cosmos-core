# Ground Station {#tutorial-gs} 

A Satellite Ground Station can be operated using COSMOS if the antenna and radio are connected to a computer capable of running COSMOS agents (ex: an ARM such a RaspberryPI or a regular PC). A microcontroller won't do it at this time.

There are at least three COSMOS agents running on the computer: agent antenna, agent radio, agent control. Agent antenna will controll the attitude of the antenna (elevation and azimuth), agent radio will control the transceiver (set frequency, compute doppler shift, etc.) and agent control if the bridge between the other two agents and the outside world, being able to receive and send C&C.

  

