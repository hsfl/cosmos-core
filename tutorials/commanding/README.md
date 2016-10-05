# COSMOS Commands {#tutorial-commanding} 

There are two mechanisms for commanding actions in COSMOS. The first involves direct commanding of Agents via their network request port. The second involves the placement of commands in the Executive Command Queue, to be run either immediately, or at some later time.

# 1. Direct Commanding

Each Agent listens for requests over a network port determined at random when it launches. This port can be discovered by listening to the heartbeats delivered by Agents at regular intervals. To access this process from the command line, make use of the "agent" command. To get a list of available Agents, use "agent list". This will display a variety of parameters about each Agent, the most important of which are their NodeName and AgentName. Each Agent running will have a unique combination of NodeName and AgentName. These can then be used to make requests of the Agent with "agent NodeName AgentName". Given in this way, with no additional arguments, it will list all available requests that can be made to the Agent. There are a certain set of requests that all Agents will respond to. Additionally, each Agent can be programmed to respond to a variety of additional requests. The function and calling format of all of these requests are available through "agent NodeName AgentName".

Example. Let's play a bit with the antenna commands. First check the antena status:

```
$ agent_client kauaicc_sim yagi get_state
[57654.363982] Cx: 1 En: 0 Target:    0.0    0.0 (   0.0    0.0) Actual: 0.0    0.0 Offset:    0.0    0.0[OK] [112]
```

Let's enable the yagi antenna rotor

```
$ agent kauaicc_sim yagi enable
```

now let's set the azimuth and elevation
```
$ agent_client kauaicc_sim yagi set_azel 1.2 3.4
```

check if it was enabled and what's the target azimuth and elevation
```
$ agent_client kauaicc_sim yagi get_state
[57654.363982] Cx: 1 En: 1 Target:    1.2    2.2 (   0.0    0.0) Actual: 1.2    2.2 Offset:    0.0    0.0[OK] [112]
```
'En: 1' means it was enabled


# 2. Executive Command Queue

The process agent_execsoh maintains a queue of commands. At a rate set by the Agent Period, this queue is searched for any commands whose execute UTC has been exceeded. Commands on the command queue are typically exectuted with 100 ms granularity. If any command in this short list has an optional condition, it is checked as well, and if zero, ignored. Finally, any commands that have already been run are ignored. Any commands left are executed, and then removed if not marked as repeating. Commands are entered into the queue as a Command Event in one of two ways: 

- First they can be sent as a request via direct commanding to agent_execsoh using "agent NodeName execsoh add_queue ...". 

- Second, they can be stored in a file with a .command extension and placed in the "incoming/exec" subfolder of the nodes folder. They will then be loaded into the command queue within the next Agent Period. 

Commands can be removed by first listing with "agent NodeName get_queue", and then removing with "agent NodeName del_queue #". Any command that is run will be logged in two separate locations. A command event record will be added to a command event file. The actual output of the command will be added to a command output file. Both files will be placed in the temp/exec subfolder of the node folder, and then periodically moved to the "outgoing/exec"subfolder.


To create commands run in bin folder
```
$ command_generator
```

Example. Let's create a command using the 'command_generator' to enable the yagi antenna, with the name 'turn_on_yagi' and command 'enable' on the mjd time '57654.3822993104':

```
$ command_generator turn_on_yagi enable 57654.3822993104
```

the result is the json formated command string that can be copied to a command file (ex: turn_on_yagi.command) and put on the incoming/exec folder (in this case it's the cosmos/nodes/kauaicc_sim/incoming/exec)

```
{"event_utc":57654.382299310397}{"event_name":"turn_on_yagi"}{"event_type":8192}{"event_flag":0}{"event_data":"agent kauaicc_sim yagi enable"}
```

note that event type 8192 represents a command event

if you have a terminal window open with the agent_soh_exec you will see that as soon as the command file is put into the incoming/exec folder the command is loaded into the queue and is displayed in the terminal window:

```
$ agent_exec_soh kauaicc_sim
Starting the executive/soh agent->..  started.
{"event_utc":57655.3822993104}{"event_utcexec":0}{"event_name":"turn_on_yagi"}{"event_type":8192}{"event_flag":0}{"event_data":"enable"}{"event_condition":"agent kauaicc_sim yagi enable"}The size of the command queue is: 1

```

if the command is waiting to be executed (is in the queue) then you can ask to see the list of commands in the queue

```
$ gent kauaicc_sim execsoh get_queue_entry
{"event_utc":57655.3822993104}{"event_utcexec":0}{"event_name":"turn_on_yagi"}{"event_type":8192}{"event_flag":0}{"event_data":"enable"}{"event_condition":"agent kauaicc_sim yagi enable"}
[OK] [163]
```

to execute the command immediatelly we just have to change the 'event_utc' time in the command file to 0

```
{"event_utc":0}{"event_name":"turn_on_yagi"}{"event_type":8192}{"event_flag":0}{"event_data":"agent kauaicc_sim yagi enable"}
```

one other alternative to generate commands is to use the 'command_generator.sh' script in the cosmos-source/core/scripts/ folder

```
./command_generator.sh 

Please enter an event name:     test
Please enter a UTC time:        0
What is the event command?      agent
Is your event repeatable? (y/n):    y
Is your event conditional? (y/n):   n
```

this will generate a 'autogen.command' file in the same folder.

one other alternative is to use the python script 'command_generator.py'

```
$ python command_generator.py
┌──────────────────────────────────────────────────────────────────────────────┐
│ ┌──────────────────────────────────────────────────────────────────────────┐ │
│ │  Super Pretty Awesome Command Event File Generator Thingy --- SPACEFGT   │ │
│ └──────────────────────────────────────────────────────────────────────────┘ │
│                    ┌─────────────────────────────────────────┐               │
│       Event Name:  │                                         │               │
│                    └─────────────────────────────────────────┘               │
│                    ┌─────────────────────┐                       ┌──┐        │
│   Event UTC Time:  │                     │        Repeatable? :  │  │        │
│                    └─────────────────────┘                       └──┘        │
│                    ┌─────────────────────────────────────────┐               │
│    Event Command:  │                                         │               │
│                    │                                         │               │
│                    │                                         │               │
│                    └─────────────────────────────────────────┘   ┌──┐        │
│                                                  Conditional? :  │  │        │
│                    ┌─────────────────────────────────────────┐   └──┘        │
│  Event Condition:  │                                         │               │
│                    └─────────────────────────────────────────┘               │
│                    ┌─────────────────────────────────────────┐               │
│  Event File Name:  │                                         │               │
│                    └─────────────────────────────────────────┘               │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

this will also generate a command on the same folder

---
These are just notes for follow up documentation, please ignore.

pending notes
- request script to send all the arguments
- conditions

future developments
- paralel commanding 
- command grouping 

