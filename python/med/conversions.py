import time
from datetime import datetime
import pytz

# Current Unix time
current_unix_time = int(time.time())
print('              Current Unix time:', current_unix_time)

# Convert Unix time to ISO 8601
iso_time = datetime.utcfromtimestamp(current_unix_time).strftime('%Y-%m-%dT%H:%M:%SZ')
print('Converted Unix time to ISO 8601:', iso_time)

# Current ISO 8601 time
current_iso_time = datetime.now(pytz.utc).strftime('%Y-%m-%dT%H:%M:%SZ')
print('               Current ISO 8601:', current_iso_time)

# Convert ISO 8601 to Unix time
dt = datetime.strptime(current_iso_time, '%Y-%m-%dT%H:%M:%SZ')
dt = pytz.utc.localize(dt)
converted_unix_time = int(dt.timestamp())
print('Converted ISO 8601 to Unix time:', converted_unix_time)
