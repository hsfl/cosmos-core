#!/usr/bin/python3

## script to visualize satellite orbit and target trajectory data

import matplotlib
matplotlib.use('TkAgg')
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
import matplotlib.cm as cm
from matplotlib.widgets import CheckButtons
from matplotlib.widgets import TextBox
from matplotlib.widgets import Slider
from astropy.time import Time


# File names for the satellite and target data
#satellite_files = ['eci_orbit_sat1.dat', 'eci_orbit_sat2.dat', 'eci_orbit_sat3.dat', 'eci_orbit_sat4.dat', 'eci_orbit_sat5.dat']
satellite_files = ['sttr/sat_1.eci', 'sttr/sat_2.eci', 'sttr/sat_3.eci', 'sttr/sat_4.eci', 'sttr/sat_0.eci']
#target_files = ['eci_target1.dat', 'eci_target2.dat', 'eci_target3.dat', 'eci_target4.dat', 'eci_target5.dat', 'eci_target6.dat']
target_files = ['sttr/target_0.eci','sttr/target_1.eci','sttr/target_2.eci','sttr/target_3.eci','sttr/target_4.eci','sttr/target_5.eci','sttr/target_6.eci','sttr/target_7.eci','sttr/target_8.eci','sttr/target_9.eci','sttr/target_10.eci','sttr/target_11.eci','sttr/target_12.eci','sttr/target_13.eci','sttr/target_14.eci','sttr/target_15.eci','sttr/target_16.eci','sttr/target_17.eci','sttr/target_18.eci','sttr/target_19.eci','sttr/target_20.eci','sttr/target_21.eci','sttr/target_22.eci','sttr/target_23.eci','sttr/target_24.eci','sttr/target_25.eci','sttr/target_26.eci','sttr/target_27.eci','sttr/target_28.eci','sttr/target_29.eci','sttr/target_30.eci','sttr/target_31.eci','sttr/target_32.eci','sttr/target_33.eci', 'sttr/target_33.eci']

# Attitude files for the satellites
att_files = ['sttr/sat_0.att', 'sttr/sat_1.att', 'sttr/sat_2.att', 'sttr/sat_3.att', 'sttr/sat_4.att']

# Load data from all files
data_sets = [np.loadtxt(file, delimiter=',') for file in satellite_files + target_files]

# Downsample the data for the animation
step = 10 # Adjust the step size to downsample data
data_sets = [data[::step] for data in data_sets]

# Load attitude data
att_datasets = [np.loadtxt(f, delimiter=',')[::step] for f in att_files]

# Length of the tail in seconds
tail_length = 5400  # Adjust the length of the tail in seconds
max_observation = 2000000 # length in meters

# Prepare the figure and 3D axis
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Earth's radii
a = 6378137  # equatorial radius in meters
b = 6356752  # polar radius in meters

# Define the Earth ellipsoid
u = np.linspace(0, 2 * np.pi, 100)
v = np.linspace(0, np.pi, 100)
x_earth = a * np.outer(np.cos(u), np.sin(v))
y_earth = a * np.outer(np.sin(u), np.sin(v))
z_earth = b * np.outer(np.ones(np.size(u)), np.cos(v))

# Plot the Earth ellipsoid
earth_plot = ax.plot_surface(x_earth, y_earth, z_earth, color='b', alpha=0.1, edgecolor='none')

# Set the aspect ratio to be equal
ax.set_box_aspect([1, 1, 1])
max_radius = a+500000  # Earth’s equatorial radius
ax.set_xlim([-max_radius, max_radius])
ax.set_ylim([-max_radius, max_radius])
ax.set_zlim([-max_radius, max_radius])

# Add a CheckButtons widget to toggle the Earth's visibility
rax = plt.axes([0.8, 0.30, 0.15, 0.05])  # Position of the CheckButtons
check = CheckButtons(rax, ['Show Earth'], [True])  # Initialize with Earth visible

# Make the border invisible by setting the spines to not visible
for spine in check.ax.spines.values():
    spine.set_visible(False)

# Alternatively, you can make the background color transparent
check.ax.set_facecolor('none')

# Define a TextBox for tail length input
tbox_ax = plt.axes([0.85, 0.25, 0.15, 0.05])  # Position of the TextBox
tail_textbox = TextBox(tbox_ax, 'Tail Length (s):', initial=str(tail_length))

# Make the border invisible by setting the spines to not visible
for spine in tail_textbox.ax.spines.values():
    spine.set_visible(False)

# Alternatively, you can make the background color transparent
tail_textbox.ax.set_facecolor('none')

# Slider for controlling animation
ax_slider = plt.axes([0.1, 0.05, 0.8, 0.03])  # Position of the slider
frame_slider = Slider(ax_slider, 'Frame', 0, len(data_sets[0]) - 1, valinit=1, valfmt='%0.0f')

# Text display for ISO time
iso_time_display = ax.text2D(0.5, 0.98, "", transform=ax.transAxes, ha='center')

# Function to toggle Earth visibility
def toggle_earth(label):
    earth_plot.set_visible(not earth_plot.get_visible())
    plt.draw()

def update_tail_length(text):
    global tail_length
    try:
        tail_length = int(text)  # Convert the input text to an integer
        plt.draw()  # Redraw the plot with the new tail length
    except ValueError:
        pass  # Ignore invalid inputs (non-integer values)

# Connect the TextBox to the update function
tail_textbox.on_submit(update_tail_length)

# Connect the checkbox to the toggle function
check.on_clicked(toggle_earth)

should_animate = False

def update(val):
    global should_animate
    # Get the current value of the slider
    frame = int(frame_slider.val)
    
    # Update the animation to start from the new frame value
    i = frame
    should_animate = True  # Set the flag to trigger animation
    
# Connect the slider to the update function
frame_slider.on_changed(update)

# Spacebar pausing

# Global variable to track paused state
paused = False
def toggle_pause(event):
    global paused
    if event.key == " ":  # Spacebar pressed
        paused = not paused

# Register the key event handler to listen for the spacebar
fig.canvas.mpl_connect('key_press_event', toggle_pause)

num_actual_sats = len(satellite_files)
num_targets = len(target_files)
# Define colors for each satellite trace and marker
num_sats = len(data_sets)
colors = plt.colormaps['tab10'].colors[:num_sats]

# Initialize lists to store the line, marker, and connection objects
lines = []
satellite_markers = []
connections = [ax.plot([], [], [], lw=1, color='black', linestyle='--')[0] for _ in range(num_actual_sats * num_targets)]
att_lines = []
for _ in att_datasets:
    att_line, = ax.plot([], [], [], 'r-', lw=2)
    att_lines.append(att_line)

# Initialize the lines, markers, and connections for each satellite and target pair
for i in range(len(data_sets)):
    line, = ax.plot([], [], [], lw=0.5, color=colors[i % len(colors)])  # Thin trace line
    marker, = ax.plot([], [], [], 'o', color=colors[i % len(colors)], lw=1)  # Satellite marker
    lines.append(line)
    satellite_markers.append(marker)
    if i < num_actual_sats:  # Only create connections for satellites to targets
        connection, = ax.plot([], [], [], lw=1.0, color='black', linestyle='--')  # Connection line
        connections.append(connection)

def init():
    for line, marker in zip(lines, satellite_markers):
        line.set_data([], [])
        line.set_3d_properties([])
        marker.set_data([], [])
        marker.set_3d_properties([])
    for connection in connections:
        connection.set_data([], [])
        connection.set_3d_properties([])
    for att_line in att_lines:
        att_line.set_data([], [])
        att_line.set_3d_properties([])
    return lines + satellite_markers + connections + att_lines


def animate(i):
    if i == len(data_sets[0])-1:
        return lines + satellite_markers + connections
    global should_animate, paused
    if paused:  # If paused, do not update the animation
        return lines + satellite_markers + connections
    # Only animate if the flag is set
    if should_animate:
        should_animate = False  # Reset the flag
        i = int(frame_slider.val)+1  # Set i to the current slider value
        i = min(i, len(data_sets[0]) - 1)

    for j, (line, marker, data) in enumerate(zip(lines, satellite_markers, data_sets)):
        x, y, z = data[:, 0], data[:, 1], data[:, 2]
        start_idx = max(0, i - tail_length // step)  # Determine the start of the tail
        # Update trace
        line.set_data(x[start_idx:i+1], y[start_idx:i+1])
        line.set_3d_properties(z[start_idx:i+1])
        # Update satellite marker
        marker.set_data(x[i:i+1], y[i:i+1])
        marker.set_3d_properties(z[i:i+1])
        # Get the MJD time for the current frame
        mjd_time = data[i, 3]
        iso_time = Time(mjd_time, format='mjd').iso
        iso_time_display.set_text(f"ISO Time: {iso_time}")

        # Update connection lines for satellites to all targets
        if j < num_actual_sats and i < len(data_sets[0]):
            for t in range(num_targets):  # Loop over all targets
                distance = np.sqrt(
                    (data_sets[j][i, 0] - data_sets[num_actual_sats + t][i, 0])**2 +
                    (data_sets[j][i, 1] - data_sets[num_actual_sats + t][i, 1])**2 +
                    (data_sets[j][i, 2] - data_sets[num_actual_sats + t][i, 2])**2
                )

                if distance < max_observation:
                    x_conn = [x[i], data_sets[num_actual_sats + t][i, 0]]
                    y_conn = [y[i], data_sets[num_actual_sats + t][i, 1]]
                    z_conn = [z[i], data_sets[num_actual_sats + t][i, 2]]
                    connections[j * num_targets + t].set_data(x_conn, y_conn)
                    connections[j * num_targets + t].set_3d_properties(z_conn)
                else:
                    # Clear the connection line
                    connections[j * num_targets + t].set_data([], [])
                    connections[j * num_targets + t].set_3d_properties([])

    for k, att_data in enumerate(att_datasets):
        sat_x, sat_y, sat_z = data_sets[k][i, 0], data_sets[k][i, 1], data_sets[k][i, 2]
        att_x, att_y, att_z = att_data[i, 0], att_data[i, 1], att_data[i, 2]

        # the proper way to get attitude vectors from attitude files
        scale = 500000.0
        att_lines[k].set_data([sat_x, sat_x + att_x*scale], [sat_y, sat_y + att_y*scale])
        att_lines[k].set_3d_properties([sat_z, sat_z + att_z*scale])

        # this quick hack/test just draws to the center of the earth based on satellite position
        #scale = 1.0
        #att_lines[k].set_data([sat_x, 0], [sat_y, 0])
        #att_lines[k].set_3d_properties([sat_z, 0])

    # Update the slider position
    frame_slider.set_val(i)
    return lines + satellite_markers + connections + att_lines

ani = animation.FuncAnimation(fig, animate, init_func=init, frames=len(data_sets[0]), interval=10, repeat=True)

plt.show()

