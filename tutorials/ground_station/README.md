# Ground Station {#tutorial-gs} 

A satellite ground station can be operated using COSMOS agents if the antenna and radio are connected to a computer capable of running a standard operating system (Linux, Mac, Win) with an ethernet interface.

There are at least three COSMOS agents running on the ground station computer: 
- \ref agent_antenna
- \ref agent_radio
- \ref agent_control

Agent antenna will controll the attitude of the antenna (elevation and azimuth), agent radio will control the transceiver (set frequency, compute doppler shift, etc.) and agent control if the bridge between the other two agents and the outside world, being able to receive and send C&C.

Let's run a ground station demo. First make sure you have the latest COSMOS/core software and COSMOS/nodes (as explained on the COSMOS 101 tutorial: https://bitbucket.org/cosmos/tutorial). We are going to be running the demo using the kauaicc_sim node. Go to the COSMOS/nodes folder (ex: /home/miguel/cosmos/nodes/kauaicc_sim) and check that you have this node installed. Also to facilitate your life make sure you have the COSMOS/bin folder populated with the three agents \ref agent_antenna, \ref agent_radio, \ref agent_control.

Let's start agent radio
```
$ agent_radio kauaicc_sim uhf
```

Now we are going to check if agent_radio is working. The following command lists all the agents running in the same network.
```
$ agent_client list
```

the previous command should return a list of the available agents similar to this:
```
[0] 57653.8367122313 kauaicc_sim uhf 192.168.56.1 57047 59996
        {"agent_pid":24732}[OK]
[1] 57653.8383072064 hiakasat nav 192.168.152.40 41311 59996
        {"agent_pid":4496}[OK]
```
  
Agent [0] is running for the 'kauacc_sim' node with the name 'uhf', with the time tag in modified Julian Days (57653.8367122313) on the address 192.168.56.1. The process id (pid) is 24732. Agent [1] is running for the 'hiakasat' node with the name 'nav'.  

Now let's start agent antenna
```
$ agent_antenna kauaicc_sim yagi
```

To print a list of commands for agent antenna execute the following command
```
$ agent_client kauaicc_sim yagi
```

This will print a large list of available requests (or commands) that can be executed remotelly on the 'yagi' agent

```
List of available requests:

        help
                list of available requests for this agent

        shutdown
                request to shutdown this agent

        idle
                request to transition this agent to idle state

        monitor
                request to transition this agent to monitor state

        run
                request to transition this agent to run state

        status
                request the status of this agent

        getvalue {"name1","name2",...}
                get specified value(s) from agent

        setvalue {"name1":value},{"name2":value},...}
                set specified value(s) in agent

        listnames
                list the Namespace of the agent

        forward nbytes packet
                Broadcast JSON packet to the default SEND port on local network

        echo utc crc nbytes bytes
                echo array of nbytes bytes, sent at time utc, with CRC crc.

        nodejson
                return description JSON for Node

        statejson
                return description JSON for State vector

        utcstartjson
                return description JSON for UTC Start time

        piecesjson
                return description JSON for Pieces

        devgenjson
                return description JSON for General Devices

        devspecjson
                return description JSON for Specific Devices

        portsjson
                return description JSON for Ports

        targetsjson
                return description JSON for Targets

        aliasesjson
                return description JSON for Aliases

        track_azel track_azel
                Supply next azimuth and elevation for tracking.

        debug debug
                Toggle debug messages.

        get_offset get_offset
                Return a report on the offset of the agent.

        get_state get_state
                Return a report on the state of the agent.

        jog jog {degrees aziumth} {degrees elevation}
                Jog the antenna azimuth and elevation in degrees.

        get_horizon get_horizon
                Return the antennas minimu elevation in degrees.

        get_azel get_azel
                Return the antenna azimuth and elevation in degrees.

        set_azel set_azel aaa.a eee.e
                Set the antenna azimuth and elevation in degrees.

        enable enable
                Enable antenna motion.

        disable disable
                Disable antenna motion.

        pause pause
                Stop where you are and make it your new target.

        stop stop
                Stop where you are, make it your new target AND disable antenna motion.

        set_offset set_offset aaa.a eee.e
                Set the antenna azimuth and elevation correction in degrees.


[OK] [2806]
```

let's now start agent control
```
$ agent_control kauaicc_sim
```

list the available antennas
```
$ agent kauaicc_sim control list_antennas
```