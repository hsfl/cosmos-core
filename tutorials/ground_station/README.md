# Ground Station {#tutorial-gs} 

A satellite ground station can be operated using COSMOS agents if the antenna and radio are connected to a computer capable of running a standard operating system (Linux, Mac, Win) with an ethernet interface.

There are at least three COSMOS agents running on the ground station computer: 
- \ref agent_antenna
- \ref agent_radio
- \ref agent_control

Agent antenna will controll the attitude of the antenna (elevation and azimuth), agent radio will control the transceiver (set frequency, compute doppler shift, etc.) and agent control if the bridge between the other two agents and the outside world, being able to receive and send C&C.

Let's run a ground station demo. First make sure you have the latest COSMOS/core software and COSMOS/nodes (as explained on the COSMOS 101 tutorial: https://bitbucket.org/cosmos/tutorial). We are going to be running the demo using the kauaicc_sim node. Go to the COSMOS/nodes folder (ex: /home/miguel/cosmos/nodes/kauaicc_sim) and check that you have this node installed. Also to facilitate your life make sure you have the COSMOS/bin folder populated with the three agents \ref agent_antenna, \ref agent_radio, \ref agent_control.

Let's start agent radio
$ agent_radio kauaicc_sim uhf



  

