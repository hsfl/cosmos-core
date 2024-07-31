#!/bin/bash
# Set the umask to 002 so that files and directories created by the file agent are modifiable by
# others in the cosmosuser (26767) group.
# This is useful when mounting volumes from the host machine to the container.
umask 002
# Start the agent
agent_file