# this Qt project combines multiple subprojects to illustrate
# agent examples

TEMPLATE = subdirs

SUBDIRS += agent_001
SUBDIRS += agent_002
SUBDIRS += agent
SUBDIRS += agent_calc

agent_001.file = agent_001.pro
agent_002.file = agent_002.pro
agent.file = agent.pro
agent_calc.file = agent_calc.pro
