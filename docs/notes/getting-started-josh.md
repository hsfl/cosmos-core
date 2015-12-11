Notes: Getting Started by Josh {#notes-getting-started-chris}
=============================

# What is Cosmos?
Cosmos is a software program that allows the user to manage and create information exchange between many machines.

# How is this done?
Cosmos uses C-object oriented programming to create the links between nodes.

# What is a node?
A node is any machine that can collect, send, or receive information. Within these nodes there must exist at least one
agent to perform operations.

# What is an agent?
An agent is an entity that exists in Cosmos that can do many different things. At the lowest level an agent basically
allows communication to another agent. It could be an agent on the same node or on another node.

# Why Cosmos?
You might be asking yourself why we would even bother with using Cosmos in the first place. Here are two examples below
that will illustrate how the agents communicate with each other and accomplish the mission objective. The example will
also hopefully give you a better understanding of the concepts of an agent and a node.

# Example 1 : Going to the light

	Mission Objective:
	A moon rover is traveling on the surface of the moon trying to collect rock samples and analyze them. The mission
	is trying to prove the past existence of water on the moon. The device used to analyze the rock however is solar
	powered. If the rover is on the dark side of the moon there is no sunlight to power the rock analyzer. Back at 
	mission control when there are enough rock samples we want to start analyzing them. If we find that our rover is
	on the dark side then we will have to start the gtl (go to light) action to bring the rover back to the light.
	
# How are we going to accomplish this?

	First let's define the parts of the mmission. There are two nodes. One is the rover on the moon and the other is the
	mission control. There are also three agents. Agents sensor, move, and done. Agent sensor and move exist on the node
	rover and agent done is on the node mission control.
	
	When we first initialize the action gtl agent done on mission control node will tell agent move to travel a predetermined
	distance. When agent move sees that the rover is done doing its task it will ask agent sensor to sense if there is any
	sunlight available. If the answer is no then agent sensor will tell agent move to move again. This will go on until agent
	sensor sees sunlight. Then instead it will communicate with agent done on mission control letting it know that gtl was
	successful. From there we can start analyzing the rocks.
	
Picture of thing

ROADMAP:

* create node for rover and ground station (we'll have to compile DMT)
    * check DMT manual (in https://bitbucket.org/cosmos/documentation)
    * check MOST manual (in https://bitbucket.org/cosmos/documentation)
* create agent for rover and ground station
* display data from rover on ground station using MOST (we'll have to compile MOST)