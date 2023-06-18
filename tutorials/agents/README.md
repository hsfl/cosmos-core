# COSMOS Agents {#tutorials-agents} 

COSMOS Agents are persistent programs that can run in almost any computer architecture (ex: Intel, Arm). With the agents you can quickly send data across different platforms to collect data from your remote sensors or even send commands to control your spacecraft. 

There are mainly two mechanisms to transfer data between agents:
* post messages
* request messages

Post messages can be divided into
* state of health (embedded/user defined)
* custom JSON (user defined)
* direct using binary or ascii (type must be > 127) (user defined)
* hearbeat (embedded/user defined)

Request messages can be divided into
* get value by request (embedded)
* request messages (user defined)


Heartbeat messages are the basic type of persistent information that each agent has. But heatbeat messages can be extended as defined by the user. State of Health (soh) messages carry larger packets of information also as defined by the user. Both heartbeats and soh messages run at a periodic interval. Request messages can be used to tailor the information exactly as requested by the user. Agent "a" sends a request to agent "b", and "b" responds with the specific information defined by the user. Requests are asynchronous and can be emitted at any time. Finally direct messages can be used to send any kind of information (including bynary) through a direct channel connected between two or more agents that know exaclty the id for the message. This gives the most flexibility to the user with the expense of development time.

The following are some tutorials to help you start using the agents: 

* @subpage tutorial-agent-talkfree
* @subpage tutorial-agent-001
* @subpage tutorial-agent-calc

