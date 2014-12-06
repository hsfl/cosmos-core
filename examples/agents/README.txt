examples/agent


new examples 2014-09-06
-----------------------
start both agents

$ ./agent_001
$ ./agent_002

try to shut them down
$ ./agent hiakasat 001 shutdown
$ ./agent hiakasat 002 shutdown

examples to set values
$ agent otb imu 'setvalue {"agent_stateflag":4}'

old
----------------------
IMPORTANT: the programs in this folder need 
'libCosmosSupport' and 'libDevice' 
to be made first (see the support/README.txt and device/README.txt)

compile the programs
$ make

start agent_cal
$ ./agent_calc 

start agent_listen
$ ./agent_listen (example run: ./agent_listen all)

query the agent 'calc' and wait for response
$ ./agent_query calc "add 2 3"


To check if there is broadcast data in the network
sudo tcpdump host 128.171.59.255
