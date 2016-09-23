#!/usr/bin/python

# script to help the user generate a command file

import curses
import curses.textpad

# draw data entry box and label (x is to the right and y is down)
def draw_box(window,l_x, l_y, l_label, d_x, d_y, d_length, d_width):
	# draw label
	window.addstr(l_y, l_x, l_label)
	# draw data entry box
	curses.textpad.rectangle(window,d_y-1,d_x-1,d_y+d_width,d_x+d_length+1)
	window.move(d_y,d_x)
	window.refresh()

# prompt the user for data
def prompt_data(window, d_x, d_y, d_length, d_width, line_separator):
	window.move(d_y,d_x)
	window.refresh()
	data_field = "";
	for width in xrange(d_width):
		if width <> d_width-1:
			data_field = data_field + window.getstr(d_y+width,d_x,d_length) + line_separator
			window.move(d_y+width,d_x)
			window.refresh()
		else:
			data_field = data_field + window.getstr(d_y+width,d_x,d_length)
			window.move(d_y+width,d_x)
			window.refresh()
	return data_field

# prompt the user for an integer
def prompt_data_integer(window, d_x, d_y, d_length, d_width, d_min, d_max):
	good_input = False
	while not good_input:
		data = prompt_data(window,d_x,d_y,d_length,d_width,'')
		good_input = data.isdigit() and d_min <= int(float(data)) <= d_max
		if not good_input:
			window.addstr(d_y, d_x, " ")
			window.addstr(d_y, d_x, "                    ")
			window.addstr(d_y, d_x+3, "("+str(d_min)+"-"+str(d_max)+")")
			window.move(d_y,d_x)
			myscreen.getch()
			window.addstr(d_y, d_x, "                    ")
	return data

# prompt the user for a menu choice (E, e, N, n, S, s, Q, q)
def prompt_menu_choice(window, d_x, d_y, d_length, d_width):
	good_input = False
	while not good_input:
		data = prompt_data(window,d_x,d_y,d_length,d_width,'')
		# check if input is good, otherwise display error
		good_input = data.isalpha() and (data=='E' or data=='N' or data=='S' or data=='Q' or data=='e' or data=='n' or data=='s' or data=='q')
		if not good_input:
			# blank out bad input
			window.addstr(d_y, d_x, " ")
	return data

# prompt the user for a yes or a no (Y, y, N, n)
def prompt_yes_or_no(window, d_x, d_y, d_length, d_width):
	good_input = False
	while not good_input:
		data = prompt_data(window,d_x,d_y,d_length,d_width,'')
		# check if input is good, otherwise display error
		good_input = data.isalpha() and (data=='Y' or data=='y' or data=='N' or data=='n')
		if not good_input:
			# blank out bad input
			window.addstr(d_y, d_x, " ")
			window.addstr(d_y, d_x+3, " (y/n)")
			window.move(d_y,d_x)
	return data


### MAIN
# Welcome Screen (prompt for type of input file)
myscreen = curses.initscr()
myscreen.keypad(1)
myscreen.border(0)
myscreen.refresh()

# draw the header
myscreen.addstr(2, 5, "Super Pretty Awesome Command Event File Generator Thingy --- SPACEFGT")
curses.textpad.rectangle(myscreen,1,2,3,77)

# set the label box for data #1
l1_x = 4;	l1_y = 5;	l1_label = "    Event Name:"
# set the input box for data #1
d1_x = 22;	d1_y = 5;	d1_length = 40;	d1_width = 1;

draw_box(myscreen,l1_x,l1_y,l1_label,d1_x,d1_y,d1_length,d1_width)

# set the label box for data #2
l2_x = 4;	l2_y =  8;	l2_label = "Event UTC Time:"
# set the input box for data #2
d2_x = 22;	d2_y =  8;	d2_length = 20;	d2_width = 1;

draw_box(myscreen,l2_x,l2_y,l2_label,d2_x,d2_y,d2_length,d2_width)

# set the label box for data #3
l3_x = 4;	l3_y = 11;	l3_label = " Event Command:"
# set the input box for data #3
d3_x = 22;	d3_y = 11;	d3_length = 40;	d3_width = 3;

draw_box(myscreen,l3_x,l3_y,l3_label,d3_x,d3_y,d3_length,d3_width)

# set the label box for data #4
l4_x = 3;	l4_y = 20;	l4_label = "Event File Name:"
# set the input box for data #4
d4_x = 22;	d4_y = 20;	d4_length = 40;	d4_width = 1;

draw_box(myscreen,l4_x,l4_y,l4_label,d4_x,d4_y,d4_length,d4_width)

# set the label box for data #5
l5_x = 46;	l5_y =  8;	l5_label = "      Repeatable? :"
# set the input box for data #5
d5_x = 68;	d5_y =  8;	d5_length = 1;	d5_width = 1;

draw_box(myscreen,l5_x,l5_y,l5_label,d5_x,d5_y,d5_length,d5_width)

# set the label box for data #6
l6_x = 46;	l6_y =  15;	l6_label = "     Conditional? :"
# set the input box for data #6
d6_x = 68;	d6_y =  15;	d6_length = 1;	d6_width = 1;

draw_box(myscreen,l6_x,l6_y,l6_label,d6_x,d6_y,d6_length,d6_width)

# set the label box for data #7
l7_x = 3;	l7_y =  17;	l7_label = "Event Condition:"
# set the input box for data #6
d7_x = 22;	d7_y =  17;	d7_length = 40;	d7_width = 1;

draw_box(myscreen,l7_x,l7_y,l7_label,d7_x,d7_y,d7_length,d7_width)


## get the data
event_name = prompt_data(myscreen,d1_x,d1_y,d1_length,d1_width,'')
event_utc = prompt_data_integer(myscreen,d2_x,d2_y,d2_length,d2_width,0,9999999999999)
event_flag_repeatable = prompt_yes_or_no(myscreen,d5_x,d5_y,d5_length,d5_width)
event_command = prompt_data(myscreen,d3_x,d3_y,d3_length,d3_width,' ')
event_flag_conditional = prompt_yes_or_no(myscreen,d6_x,d6_y,d6_length,d6_width)

event_condition = prompt_data(myscreen,d7_x,d7_y,d7_length,d7_width,'')
event_file_name = prompt_data(myscreen,d4_x,d4_y,d4_length,d4_width,'')


# make the event_type
event_type=8192

# make the event_flag
event_flag=0
if event_flag_repeatable == 'Y' or event_flag_repeatable == 'y':
	event_flag = event_flag + 131072
if event_flag_conditional == 'Y' or event_flag_conditional == 'y':
	event_flag = event_flag + 65536



# Exit Screen
myscreen.getch()
myscreen.keypad(0)
curses.endwin()


# write the file
f = open(event_file_name+".command", 'w')


f.write('{"event_name":"')
f.write(event_name)
f.write('"}')

f.write('{"event_utc":')
f.write(event_utc)
f.write('}')

f.write('{"event_utcexec":0}')

f.write('{"event_flag":')
f.write(str(event_flag))
f.write('}')

f.write('{"event_type":')
f.write(str(event_type))
f.write('}')


f.write('{"event_data":"')
f.write(event_command.rstrip().replace('"','\\"'))
f.write('"}')

f.write('{"event_condition":"')
f.write(event_condition)
f.write('"}')

f.write('\n')

f.close()

print '<'+event_name+'>'
print '<'+str(event_utc)+'>'
print '<'+event_flag_repeatable+'>'
print '<'+event_flag_conditional+'>'
print '<'+str(event_flag)+'>'
print '<'+event_command.rstrip().replace('"','\\"')+'>'
print '<'+event_condition+'>'
print '<'+event_file_name+'>'
