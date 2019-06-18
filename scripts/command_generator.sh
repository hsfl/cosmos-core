#!/bin/bash

# Two methods of input: command line arguments, or through the command-line interface.

if [ $# -eq 1 ] || [ $# -gt 5 ]; then
	echo "Usage: command_generator.sh [name command [time [condition repeat_flag]]]"
    exit 1

elif [ $# -ge 2 ] && [ $# -le 5 ]; then
	event_name=$1
	event_data=$2
	event_utc=${3:-0}
	event_utcexec=0
	event_type=8192
    event_flag_rep=${5:-n}
    event_condition=${4:""}

    if ! [ "$event_flag_rep" == 'Y' -o "$event_flag_rep" == 'y' -o "$event_flag_rep" == 'N' -o "$event_flag_rep" == 'n' ]; then
        echo "Please enter (y/n) for the repeat_flag."
        exit 1
    fi

else
    ## script to help the user generate commands for agent_exec
    echo

    ### what is the event name?
    echo -e "Please enter an event name:\t\t\c"
    read event_name

    ### what is the event time?
    echo -e "Please enter a UTC time:\t\t\c"
    read event_utc

    ### what is the event command?
    echo -e "What is the event command?\t\t\c"
    read event_data

    # this gets set once the command has executed
    event_utcexec=0

    # event type (8192 represents a command event)
    event_type=8192

    ### is the event repeatable?

    BAD_INPUT=1
    while [ $BAD_INPUT -eq 1 ]
    do
      echo -e "Is your event repeatable? (y/n):\t\c"
      read event_flag_rep
      if [ "$event_flag_rep" == 'Y' -o "$event_flag_rep" == 'y' -o "$event_flag_rep" == 'N' -o "$event_flag_rep" == 'n' ]
      then
        BAD_INPUT=0
      fi
    done

    ### is the event conditional?

    BAD_INPUT=1
    while [ $BAD_INPUT -eq 1 ]
    do
      echo -e "Is your event conditional? (y/n):\t\c"
      read event_flag_con
      if [ "$event_flag_con" == 'Y' -o "$event_flag_con" == 'y' -o "$event_flag_con" == 'N' -o "$event_flag_con" == 'n' ]
      then
        BAD_INPUT=0
      fi
    done
fi

if [ "$event_flag_rep" == 'Y' -o "$event_flag_rep" == 'y' ]
then
  event_flag_rep_num=131072
else
  event_flag_rep_num=0
fi

if [ "$event_flag_con" == 'Y' -o "$event_flag_con" == 'y' ]
then
  event_flag_con_num=65536
else
  event_flag_con_num=0
fi

### set the event flag
event_flag=`echo $event_flag_rep_num+$event_flag_con_num | bc`

### if the event is conditional, what is the condition?
if [ "$event_flag_con" == 'Y' -o "$event_flag_con" == 'y' ]
then
  echo -e "What is the event condition?:\t\t\c"
  read event_condition
fi

filename=autogen.command
rm -f "$filename"

### print out the command event in JSON syntax to stdout and to the file.
echo -e "{\"event_name\":\"$event_name\"}\c" | tee -a "$filename"
echo -e "{\"event_utc\":$event_utc}\c" | tee -a "$filename"
echo -e "{\"event_utcexec\":$event_utcexec}\c" | tee -a "$filename"
echo -e "{\"event_flag\":$event_flag}\c" | tee -a "$filename"
echo -e "{\"event_type\":$event_type}\c" | tee -a "$filename"
echo -e "{\"event_data\":\"$event_data\"}\c" | tee -a "$filename"
echo -e "{\"event_condition\":\"$event_condition\"}\c" | tee -a "$filename"
echo | tee -a "$filename"
