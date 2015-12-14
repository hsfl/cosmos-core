# Agent Calc {#tutorial-agent-calc} 

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

old note: IMPORTANT: the programs in this folder need 
'libCosmosSupport' and 'libDevice' 
to be made first (see the support/README.txt and device/README.txt)
