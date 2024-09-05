import json
import streamlit as st
import pandas as pd
from st_aggrid import AgGrid
import plotly.graph_objects as go
import plotly.express as px
import time
from datetime import datetime, timedelta

st.set_page_config(layout="wide", initial_sidebar_state="auto")


# CSS for styling improvements
st.markdown(
    """
    <style>
    .stApp {
        background-color: #F5F7FA; /* Light background for better contrast */
        padding: 20px;
    }
    .stTitle {
        font-size: 36px;
        color: #333333;
        font-weight: bold;
        margin-bottom: 20px;
    }
    .stTextInput, .stButton, .stCheckbox, .stExpander {
        margin: 10px 0;
    }
    .stButton>button {
        background-color: #007BFF;
        color: #FFFFFF;
        border-radius: 5px;
        padding: 10px 20px;
        border: none;
    }
    .stButton>button:hover {
        background-color: #0056b3; /* Darker blue on hover */
    }
    .stTextInput>input, .stSelectbox>select, .stCheckbox>label, .stButton>button {
        font-size: 14px;
        padding: 8px;
        margin: 5px 0;
    }
    .stExpanderHeader {
        background-color: #E8EAF6; /* Subtle blue for expander headers */
        padding: 10px;
        border-radius: 5px;
    }
    .stSlider {
        padding: 0 10px;
    }
    </style>
    """,
    unsafe_allow_html=True
)


def load_json(file_path):
    try:
        with open(file_path, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        st.error(f"File not found: {file_path}")
        return {}
    except json.JSONDecodeError:
        st.error(f"Error decoding JSON from file: {file_path}")
        return {}

EVENT_DICTIONARY_PATH = "event_dictionary.json"
EVENTS_TIMELINE_PATH = "events_timeline.json"

event_dictionary = load_json(EVENT_DICTIONARY_PATH)
events_timeline = load_json(EVENTS_TIMELINE_PATH)

# Function to prevent automatic scrolling with time bar simulator
def no_scroll():
    script = """
    <script>
        const slider = document.querySelector('.stSlider');
        slider.addEventListener('input', function(event) {
            event.preventDefault();
        });
    </script>
    """
    st.write(script, unsafe_allow_html=True)


def add_seconds(timestamp_str, duration):
    format_str = "%Y-%m-%dT%H:%M:%SZ"
    timestamp = datetime.strptime(timestamp_str, format_str)
    new_timestamp = timestamp + timedelta(seconds=duration)
    new_timestamp_str = new_timestamp.strftime(format_str)
    return new_timestamp_str


def get_hour_minute_second(timestamp_str):
    format_str = "%Y-%m-%dT%H:%M:%SZ"
    timestamp = datetime.strptime(timestamp_str, format_str)
    time_str = timestamp.strftime("%H:%M:%S")
    return time_str


def get_month_day_year(timestamp_str):
    format_str = "%Y-%m-%dT%H:%M:%SZ"
    timestamp = datetime.strptime(timestamp_str, format_str)
    time_str = timestamp.strftime("%Y-%m-%d")
    return time_str


def combine_both_times(hour_minute_second, month_day_year):
    combined_str = f"{month_day_year}T{hour_minute_second}Z"
    return combined_str


def to_datetime(timestamp_str):
    format_str = "%Y-%m-%dT%H:%M:%SZ"
    return datetime.strptime(timestamp_str, format_str)


def time_diff_in_seconds(start, end):
    return int((end - start).total_seconds())


def seconds_to_time_format(seconds):
    hours, remainder = divmod(seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    return f"{int(hours):02}:{int(minutes):02}:{int(seconds):02}"


events = []
for event in events_timeline:
    event_id = event["event_id"]
    if event_id in event_dictionary:
        duration = event_dictionary[event_id]["duration"]
        start_seconds = event['start_time']
        end_seconds = add_seconds(start_seconds, duration)
        events.append({
            "start_time": start_seconds,
            "end_time": end_seconds,
            "event_name": event_id,
        })
events_json = {"events": events}

events_df = pd.DataFrame(events)
fig = go.Figure()

# Unique event names
unique_event_names = events_df['event_name'].unique()

# Color scale and mapping
color_scale = px.colors.qualitative.Plotly
color_mapping = {event_name: color_scale[i % len(
    color_scale)] for i, event_name in enumerate(unique_event_names)}

if 'hash_map' not in st.session_state:
    st.session_state.hash_map = {}
    checkbox = False
    for i, val in enumerate(events):
        st.session_state.hash_map[i] = [val['event_name'],
                                        val['start_time'],
                                        val['end_time'],
                                        True]

if 'edit_mode' not in st.session_state:
    st.session_state.edit_mode = {i: False for i in range(len(events))}

with st.expander("Edit Events"):
    num_events_per_row = 5
    total_events = len(st.session_state.hash_map)
    for i in range(total_events):
        if i % num_events_per_row == 0:
            columns = st.columns(num_events_per_row)
        event_name = st.session_state.hash_map[i][0]
        with columns[i % num_events_per_row]:
            st.session_state.hash_map[i][3] = st.checkbox(
                event_name,  key=f"checkbox1_{i}", value=True)
            start_time_hour_minute_second = get_hour_minute_second(
                st.session_state.hash_map[i][1])
            end_time_hour_minute_second = get_hour_minute_second(
                st.session_state.hash_map[i][2])
            st.write("Start:", start_time_hour_minute_second)
            st.write("End:", end_time_hour_minute_second)
            if st.button("Edit", key=f"edit_{i}"):
                st.session_state.edit_mode[i] = not st.session_state.edit_mode[i]
            if st.session_state.edit_mode[i]:
                start_time_month_day_year = get_month_day_year(
                    st.session_state.hash_map[i][1])
                end_time_month_day_year = get_month_day_year(
                    st.session_state.hash_map[i][2])
                start_time_edit = st.text_input(
                    "Edit Start Time", value=start_time_hour_minute_second,  key=f"start_input_{i}")
                end_time_edit = st.text_input(
                    "Edit End Time", value=end_time_hour_minute_second, key=f"end_input_{i}")
                if st.button("Save Changes", key=f"save_{i}"):
                    new_start_time = combine_both_times(
                        start_time_edit, start_time_month_day_year)
                    new_end_time = combine_both_times(
                        end_time_edit, end_time_month_day_year)
                    st.session_state.hash_map[i][1] = new_start_time
                    st.session_state.hash_map[i][2] = new_end_time
                    st.session_state.edit_mode[i] = False
                    st.success("Changes Saved!")
                elif st.button("Cancel", key=f"cancel_{i}"):
                    st.session_state.edit_mode[i] = False

# Calculate x-positions and adjust layout width
step_size = 0.3
x_positions = [i * step_size for i in range(len(events))]
margin_offset = 1.0  # Adjust margin to ensure no bars are cut off

for i in range(len(events)):
    offset = 0.01
    opacity_value = 0.35 if i % 2 == 1 else 1
    if st.session_state.hash_map[i][3]:
        fig.add_trace(go.Scatter(
            x=[x_positions[i] + offset, x_positions[i] + offset],
            y=[st.session_state.hash_map[i][1],
                st.session_state.hash_map[i][2]],
            line=dict(
                width=15, color=color_mapping[st.session_state.hash_map[i][0]]),
            name=st.session_state.hash_map[i][0] if i == 0 else None,
            mode='lines',
            hoverinfo='text',
            opacity=opacity_value,
            hovertext=f"{st.session_state.hash_map[i][0]}<br>Start: {get_hour_minute_second(st.session_state.hash_map[i][1])}<br>End: {get_hour_minute_second(st.session_state.hash_map[i][2])}"
        ))
    offset += 1.5

# Iterate over unique event names
color_coded_ticktext = [f'<span style="color:{color_scale[i % len(color_scale)]}">{" "}{name}</span>'
                            for i, name in enumerate(unique_event_names) if st.session_state.hash_map[i][0]]

# Initialize or retrieve current y-axis limit from session state
if 'y_axis_limit' not in st.session_state:
    st.session_state.y_axis_limit = 3000  # Set the initial y-axis limit to 3000

if 'dtick' not in st.session_state:
    st.session_state.dtick = 3600000

if 'y_axis_spacing' not in st.session_state:
    st.session_state.y_axis_spacing = 600

zoom_levels = {
    43200000: (21600000, 50), # 12 hours to 6 hours
    21600000: (10800000, 50),  # 6 hours to 3 hours
    10800000: (3600000, 50),   # 3 hours to 1 hour
    3600000: (1800000, 100),  # 1 hour to 30 minutes
    1800000: (900000, 100),   # 30 minutes to 15 minutes
    900000: (300000, 100),     # 15 minutes to 5 minutes
    300000: (60000, 100)       # 5 minutes to 1 minute
}

col1, col2 = st.columns([1, 12])

reverse_zoom_levels = {v[0]: (k, -v[1]) for k, v in zoom_levels.items()}

    # Initialize session state if not already initialized
if 'play' not in st.session_state:
    st.session_state.play = False
if 'slider_value' not in st.session_state:
    st.session_state.slider_value = 0
if 'scroll_value' not in st.session_state:
    st.session_state.scroll_value = 0
if 'scroll_position' not in st.session_state:
    st.session_state.scroll_position = 0
if 'slider_max_value' not in st.session_state:
    st.session_state.slider_max_value = 86400 

no_scroll()

earliest_start_time_str = events_df['start_time'].min()
earliest_start_time = datetime.strptime(
    earliest_start_time_str, "%Y-%m-%dT%H:%M:%SZ")
one_hour_before_start_time = earliest_start_time - \
    timedelta(hours=1)
y_position = one_hour_before_start_time + \
    timedelta(seconds=st.session_state.slider_value)

latest_end_time_str = events_df['end_time'].max()
latest_end_time = datetime.strptime(
    latest_end_time_str, "%Y-%m-%dT%H:%M:%SZ")
slider_max_value = int(
    (latest_end_time - earliest_start_time).total_seconds() + 7200)

y_axis_increments = {
    #12 hours 
    43200000: (y_position - timedelta(hours=144), y_position + timedelta(hours=144)),
    # 6 hours
    21600000: (y_position - timedelta(hours=72), y_position + timedelta(hours=72)),
    # 3 hours
    10800000: (y_position - timedelta(hours=36), y_position + timedelta(hours=36)),
    # 1 hour
    3600000: (y_position - timedelta(hours=12), y_position + timedelta(hours=12)),
    # 30 minutes
    1800000: (y_position - timedelta(hours=6), y_position + timedelta(hours=6)),
    # 15 minutes
    900000: (y_position - timedelta(hours=3), y_position + timedelta(hours=3)),
    # 5 minutes
    300000: (y_position - timedelta(hours=1), y_position + timedelta(hours=1)),
    #1 minute
    60000: (y_position - timedelta(minutes=12), y_position + timedelta(minutes=12))
}

with col1:
    if st.button("Zoom In"):
        current_dtick = st.session_state.dtick
        if current_dtick in zoom_levels:
            new_dtick, y_axis_change = zoom_levels[current_dtick]
            st.session_state.dtick = new_dtick
            st.session_state.y_axis_spacing += y_axis_change
        else:
            st.warning("Cannot zoom in more")

with col2:
    if st.button("Zoom out"):
        current_dtick = st.session_state.dtick
        if current_dtick in reverse_zoom_levels:
            new_dtick, y_axis_change = reverse_zoom_levels[current_dtick]
            st.session_state.dtick = new_dtick
            st.session_state.y_axis_spacing += y_axis_change
        else:
            st.warning("Cannot zoom out more")
y_start, y_end = y_axis_increments[st.session_state.dtick]

    # Update the layout with the dynamic y-axis limits
fig.update_layout(
        title='Mission Events Display',        
        title_x=0.5,
        title_y=1.00,  # Move the title up (0.95 means it's closer to the top)
        font=dict(
                family="Arial, sans-serif",
                size=24,
                color='#003366'
            ),
        xaxis=dict(
            tickvals=[i for i, name in enumerate(
                unique_event_names) if st.session_state.hash_map[i][0]],
            ticktext=color_coded_ticktext,
            side="top",
            tickangle=0,
            range=[-margin_offset, len(unique_event_names) + 1 + margin_offset]
        ),
        yaxis=dict(
            title='Time (minutes)',
            tickformat="%H:%M:%S",
            range=[y_start, y_end],
            dtick=st.session_state.dtick
        ),
    
        height=500,
        showlegend=False,
        margin=dict(l=20, r=20, t=20, b=20)
)

sorted_events = sorted(events, key=lambda x: datetime.strptime(
        x['start_time'], "%Y-%m-%dT%H:%M:%SZ"))

    # Finds the next event block after the current position of the green bar
next_event_index = 0
for i in range(len(sorted_events)):
    event_start_time = datetime.strptime(
    sorted_events[i]['start_time'], "%Y-%m-%dT%H:%M:%SZ")
    if event_start_time > y_position:
        next_event_index = i
        break

    # Retrieve the next event's information
next_event_name = sorted_events[next_event_index]['event_name']
next_event_start = datetime.strptime(
    sorted_events[next_event_index]['start_time'], "%Y-%m-%dT%H:%M:%SZ")
time_until_next_event = next_event_start - y_position

if time_until_next_event < timedelta(seconds=0):
    next_event_name = "No more events"
    time_until_next_event = "N/A"
else:
    # Format the time until the next event as needed
    time_until_next_event_str = str(time_until_next_event)

    # Time Bar (fixed in the middle)
fig.add_shape(
        type="line",
        x0=-1,
        y0=y_position,
        x1=len(unique_event_names) + 6.5,
        y1=y_position,
        line=dict(
            color="green",
            width=2,
        )
)
    
formatted_time = seconds_to_time_format(st.session_state.slider_value)# Calculates the position for the countdown timer
timer_position = len(unique_event_names) + 0.5

# Annotation for the countdown timer
fig.add_annotation(
        x=timer_position,
        y=y_position,
        xshift=-40,
        yshift=10,
        text=f"<b>Current Time:</b> {formatted_time}",
        showarrow=False,
        font=dict(
            size=14,
            color="green",
        ),
)

    
if next_event_name == "No more events":
    next_event_text = f"<b>No Upcoming Events</b><br>(Current Time: {formatted_time})"
else:
    next_event_text = (
    f"<b>Time Until Next Event: </b> {time_until_next_event}"
    f"<br>"
    f"<b>Next Event: </b> {next_event_name}"
)
        
            
fig.add_annotation(
        x=timer_position,
        y=y_position,
        text=next_event_text,
        yshift= -200,
        xshift= 180,
        showarrow=False,
        font=dict(
            size=14,
            color="green",
        ),
        align="right",  # Aligns text to the right
            xanchor="right",  # Anchors the x position to the right edge of the text box
)
    

st.plotly_chart(fig, use_container_width=True)



    # Slider for time bar simulation
slider_value = st.slider("**Time Bar Simulation**", min_value=0, max_value=slider_max_value,
                             value=st.session_state.slider_value, step=1, key='slider')

    # Update session state with the current slider value
st.session_state.slider_value = slider_value

    # Converts slider value to HH:MM:SS format
st.write(f"Slider Value: {slider_value}")

    # Layout for Play/Pause and Reset buttons
col1, col2, _ = st.columns([1, 1, 8])

with col1:
    if st.button('Play/Pause'):
        st.session_state.play = not st.session_state.play

    # Reset button
with col2:
    if st.button('Reset'):
        st.session_state.slider_value = 0
        st.session_state.play = False
        st.session_state.scroll_position = 0
        st.rerun()
        

     # JavaScript to capture the scroll position
capture_scroll_position = """
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            window.scrollTo(0, %d);
        });
        window.addEventListener('scroll', function() {
            var scrollPosition = window.scrollY;
            fetch('/scroll_position', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({scrollPosition: scrollPosition}),
            });
        });
    </script>
    """ % st.session_state.scroll_position
st.write(capture_scroll_position, unsafe_allow_html=True)

   # Endpoint to save the scroll position
def save_scroll_position():
    current_params = st.query_params  # Retrieve the current query parameters

        # Check if 'scroll_position' exists, if not, set it to 0
    if 'scroll_position' not in current_params:
        current_params['scroll_position'] = ['0']  # Set the scroll position to 0

        # Retrieve the scroll position from the query parameters
    scroll_position = current_params.get('scroll_position', ['0'])[0]
    st.session_state.scroll_position = int(scroll_position)  # Save to session state

        # Update the query parameters with the new scroll position
    st.query_params.update(current_params)

save_scroll_position()

def update_slider():
    while st.session_state.play:
        if st.session_state.slider_value < st.session_state.slider_max_value:
            st.session_state.slider_value += 300  # Increase by 1 minute or adjust as needed
        else:
            st.session_state.play = False  # Stop playing if the slider reaches the max value
        time.sleep(1)  # Adjust the speed of the slider
        st.rerun()  # Refresh the app to update the slider value

    # Call the function if play is active
if st.session_state.play:
    update_slider()
