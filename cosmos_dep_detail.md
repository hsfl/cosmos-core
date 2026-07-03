# COSMOS Core — Symbol-Level Dependency Detail

> For each (file, include) pair, lists every exported symbol from that header
> and marks whether it is referenced in the including file.

---

## agent

### `agentclass.cpp`

**`agent/agentclass.h`** — 151 used / 164 exported

*Used:* `Agent`, `AgentMessage`, `State`, `Where`, `add_device`, `add_request`, `channel_add`, `channel_age`, `channel_bytes`, `channel_clear`, `channel_count`, `channel_datasize`, `channel_enable`, `channel_enabled`, `channel_increment`, `channel_level`, `channel_maximum`, `channel_name`, `channel_number`, `channel_packets`, `channel_pull`, `channel_push`, `channel_rawsize`, `channel_size`, `channel_speed`, `channel_teststart`, `channel_teststop`, `channel_touch`, `channel_update`, `channel_wakeup_timer` … (+121 more)

**`support/cosmos-errno.h`** — 1 used / 1 exported

*Used:* `ErrorNumbers`

**`support/stringlib.h`** — 10 used / 44 exported

*Used:* `from_hex_string`, `string_find`, `string_join`, `string_split`, `to_floating`, `to_hex`, `to_json`, `to_label`, `to_mjd`, `to_unsigned`

**`support/timelib.h`** — 8 used / 61 exported

*Used:* `centisec`, `currentmjd`, `decisec`, `mjd2iso8601`, `mjdToGregorian`, `secondsleep`, `to_iso8601`, `utc2unixseconds`

**`support/socketlib.h`** — 4 used / 37 exported

*Used:* `NetworkType`, `sockaddr_in`, `socket_channel`, `socket_open`

**`support/elapsedtime.h`** — 5 used / 16 exported

*Used:* `ElapsedTime`, `reset`, `split`, `start`, `stop`


### `agentclass.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/logger.h`** — 1 used / 11 exported

*Used:* `Logger`

**`support/jsonlib.h`** — 0 used / 204 exported

*Exported but none matched:* `add_node_id`, `calc_events`, `change_node_id`, `check_node_id`, `create_databases`, `device_component_index`, `device_has_property`, `device_index`, `device_type_index`, `device_type_name`, `json_adddevice`, `json_addentry`, `json_addpiece`, `json_append`, `json_checkentry`, `json_clear_cosmosstruc`, `json_clone_node`, `json_convert_double`, `json_convert_float`, `json_convert_int16` … (+184 more)

**`support/jsonclass.h`** — 1 used / 20 exported

*Used:* `Json`

**`device/cpu/devicecpu.h`** — 1 used / 43 exported

*Used:* `DeviceCpu`

**`support/packetcomm.h`** — 2 used / 22 exported

*Used:* `PacketComm`, `TypeId`

**`support/channellib.h`** — 2 used / 33 exported

*Used:* `Channel`, `start`

**`support/beacon.h`** — 1 used / 29 exported

*Used:* `TypeId`

**`task.h`** — 2 used / 14 exported

*Used:* `State`, `Task`


### `command_queue.cpp`

**`agent/command_queue.h`** — 13 used / 18 exported

*Used:* `CommandQueue`, `add_command`, `advance`, `begin`, `del_command`, `load_commands`, `restore_commands`, `run_command`, `run_commands`, `run_request`, `save_commands`, `size`, `sort`


### `command_queue.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`agentclass.h`** — 1 used / 164 exported

*Used:* `Agent`

**`task.h`** — 2 used / 14 exported

*Used:* `Running`, `Task`

**`event.h`** — 2 used / 21 exported

*Used:* `Event`, `getTime`


### `event.cpp`

**`agent/event.h`** — 5 used / 21 exported

*Used:* `Event`, `condition_true`, `generator`, `get_event_string`, `set_command`

**`support/jsonobject.h`** — 3 used / 7 exported

*Used:* `JSONObject`, `addElement`, `to_json_string`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)


### `event.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jsonlib.h`** — 0 used / 204 exported

*Exported but none matched:* `add_node_id`, `calc_events`, `change_node_id`, `check_node_id`, `create_databases`, `device_component_index`, `device_has_property`, `device_index`, `device_type_index`, `device_type_name`, `json_adddevice`, `json_addentry`, `json_addpiece`, `json_append`, `json_checkentry`, `json_clear_cosmosstruc`, `json_clone_node`, `json_convert_double`, `json_convert_float`, `json_convert_int16` … (+184 more)

**`support/timelib.h`** — 2 used / 61 exported

*Used:* `currentmjd`, `mjd2iso8601`


### `scheduler.cpp`

**`agent/scheduler.h`** — 5 used / 5 exported

*Used:* `Scheduler`, `addEvent`, `deleteEvent`, `getEventQueue`, `getEventQueueSize`


### `scheduler.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`agentclass.h`** — 1 used / 164 exported

*Used:* `Agent`

**`event.h`** — 1 used / 21 exported

*Used:* `Event`


### `task.cpp`

**`task.h`** — 13 used / 14 exported

*Used:* `Add`, `Command`, `Deci`, `Del`, `Exists`, `Iretn`, `Path`, `Runner`, `Size`, `Start`, `Startmjd`, `State`, `Task`

**`support/stringlib.h`** — 1 used / 44 exported

*Used:* `string_replace`

**`support/elapsedtime.h`** — 2 used / 16 exported

*Used:* `ElapsedTime`, `split`


### `task.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/datalib.h`** — 0 used / 62 exported

*Exported but none matched:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive` … (+42 more)

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)


## device/arduino

### `arduino_lib.cpp`

**`support/configCosmos.h`** — 3 used / 12 exported

*Used:* `COSMOS_USLEEP`, `close`, `defined`

**`device/arduino/arduino_lib.h`** — 12 used / 12 exported

*Used:* `arduino_closeport`, `arduino_delay`, `arduino_init`, `arduino_printnum`, `arduino_printstring`, `arduino_read`, `arduino_setbaud`, `arduino_setport`, `serialport_init`, `serialport_read_until`, `serialport_write`, `serialport_writebyte`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `secondsleep`


### `arduino_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


## device/ccsds

### `spp.cpp`

**`spp.h`** — 29 used / 31 exported

*Used:* `PacketStage`, `PacketType`, `SequenceFlags`, `Spp`, `addByte`, `addDataByte`, `clearDataBytes`, `clearPacket`, `getApid`, `getDataBytes`, `getDataLength`, `getFrame`, `getHeaderBytes`, `getSecondaryHeaderFlag`, `getSequenceCount`, `getSequenceFlags`, `getType`, `getVersion`, `packet`, `setApid`, `setApidIdle`, `setDataBytes`, `setDataLength`, `setHeaderByte`, `setSecondaryHeaderFlag`, `setSequenceCount`, `setSequenceFlags`, `setType`, `setVersion`


### `spp.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


## device/cpu

### `devicecpu.cpp`

**`device/cpu/devicecpu.h`** — 43 used / 43 exported

*Used:* `BytesToKiB`, `BytesToMB`, `BytesToMiB`, `CalculateCPULoad`, `DeviceCpu`, `DeviceCpuLinux`, `DeviceCpuMac`, `DeviceCpuWindows`, `FileTimeToInt64`, `defined`, `getBootCount`, `getCpuCount`, `getCurrentProcessName`, `getHostName`, `getLoad`, `getLoad1minAverage`, `getMemoryTotal`, `getMemoryTotalKiB`, `getMemoryTotalMB`, `getMemoryTotalMiB`, `getMemoryUsed`, `getMemoryUsedKiB`, `getMemoryUsedMB`, `getMemoryUsedMiB`, `getMemoryUsedOf`, `getPercentCpuOf`, `getPercentMemoryOf`, `getPercentUseForCurrentProcess`, `getPercentUseForCurrentProcessOverLifetime`, `getPidOf` … (+13 more)


### `devicecpu.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `defined`

**`support/stringlib.h`** — 0 used / 44 exported

*Exported but none matched:* `StringParser`, `byte_vector_to_string`, `clean_string`, `from_hex`, `from_hex_string`, `from_hex_vector`, `getFieldNumber`, `getFieldNumberAsDouble`, `getFieldNumberAsInteger`, `splitString`, `string_cmp`, `string_find`, `string_join`, `string_parse`, `string_replace`, `string_split`, `string_to_byte_vector`, `to_angle`, `to_astring`, `to_binary` … (+24 more)

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`support/datalib.h`** — 0 used / 62 exported

*Exported but none matched:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive` … (+42 more)

**`windows.h`** — header not indexed (third-party or missing)


## device/disk

### `devicedisk.cpp`

**`device/disk/devicedisk.h`** — 11 used / 11 exported

*Used:* `DeviceDisk`, `defined`, `getAll`, `getFree`, `getFreeGiB`, `getInfo`, `getSize`, `getSizeGiB`, `getUsed`, `getUsedGiB`, `info`


### `devicedisk.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `defined`

**`support/stringlib.h`** — 0 used / 44 exported

*Exported but none matched:* `StringParser`, `byte_vector_to_string`, `clean_string`, `from_hex`, `from_hex_string`, `from_hex_vector`, `getFieldNumber`, `getFieldNumberAsDouble`, `getFieldNumberAsInteger`, `splitString`, `string_cmp`, `string_find`, `string_join`, `string_parse`, `string_replace`, `string_split`, `string_to_byte_vector`, `to_angle`, `to_astring`, `to_binary` … (+24 more)

**`windows.h`** — header not indexed (third-party or missing)


## device/general

### `acq_a35.cpp`

**`agent/agentclass.h`** — 1 used / 164 exported

*Used:* `start`

**`device/general/gige_lib.h`** — 7 used / 44 exported

*Used:* `gige_address_to_value`, `gige_close`, `gige_discover`, `gige_handle`, `gige_open`, `gige_value_to_address`, `gige_writereg`

**`device/general/acq_a35.h`** — 11 used / 12 exported

*Used:* `AutoFFC`, `DoFFC`, `GetCameraFrame`, `GetTemperature`, `InitCamera`, `ManualFFC`, `ResetCamera`, `SetFramesToSkip`, `SetPacketDelay`, `StartCamera`, `StopCamera`

**`support/elapsedtime.h`** — 4 used / 16 exported

*Used:* `ElapsedTime`, `lap`, `split`, `start`


### `acq_a35.h`

**`device/general/gige_lib.h`** — 1 used / 44 exported

*Used:* `gige_handle`


### `bbFctns.cpp`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `COSMOS_USLEEP`

**`device/general/cssl_lib.h`** — 7 used / 24 exported

*Used:* `cssl_close`, `cssl_drain`, `cssl_getdata`, `cssl_geterrormsg`, `cssl_open`, `cssl_putstring`, `cssl_settimeout`

**`device/general/bbFctns.h`** — 3 used / 3 exported

*Used:* `FindBbPort`, `OpenBb`, `runBbUnit1`


### `cssl_lib.cpp`

**`support/configCosmos.h`** — 3 used / 12 exported

*Used:* `COSMOS_USLEEP`, `close`, `defined`

**`device/general/cssl_lib.h`** — 21 used / 24 exported

*Used:* `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup`, `cssl_start`, `cssl_stop`, `defined`


### `cssl_lib.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `defined`

**`support/sliplib.h`** — 0 used / 6 exported

*Exported but none matched:* `slip_calc_crc`, `slip_decode`, `slip_encode`, `slip_extract`, `slip_pack`, `slip_unpack`


### `gige_lib.cpp`

**`device/general/gige_lib.h`** — 34 used / 44 exported

*Used:* `GigeAcquisitionMode`, `GigeFormat`, `PHXReg`, `PT1000`, `a35_config`, `a35_image`, `gige_address_to_value`, `gige_close`, `gige_data`, `gige_discover`, `gige_handle`, `gige_open`, `gige_read_float`, `gige_read_int32`, `gige_read_string`, `gige_read_uint32`, `gige_readmem`, `gige_readreg`, `gige_readreg2`, `gige_request`, `gige_value_to_address`, `gige_writereg`, `phx_config`, `phx_image`, `prosilica_config`, `prosilica_image`, `pt1000_config`, `pt1000_drain`, `pt1000_image`, `pt1000_image_dark` … (+4 more)

**`support/timelib.h`** — 2 used / 61 exported

*Used:* `currentmjd`, `secondsleep`

**`math/mathlib.h`** — 1 used / 78 exported

*Used:* `size`

**`support/elapsedtime.h`** — 3 used / 16 exported

*Used:* `ElapsedTime`, `reset`, `split`


### `gige_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/socketlib.h`** — 1 used / 37 exported

*Used:* `socket_channel`

**`math/bytelib.h`** — 0 used / 17 exported

*Exported but none matched:* `doublefrom`, `doubleto`, `floatfrom`, `floatto`, `int16from`, `int16to`, `int32from`, `int32to`, `local_byte_order`, `uint16from`, `uint16to`, `uint32from`, `uint32to`, `uint8from`, `uint8to`, `uintswap`, `vector8invert`

**`math/vector.h`** — 3 used / 148 exported

*Used:* `Vector`, `length`, `size`


### `gs232b_lib.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/gs232b_lib.h`** — 16 used / 17 exported

*Used:* `gs232b_az_speed`, `gs232b_connect`, `gs232b_disconnect`, `gs232b_get_az`, `gs232b_get_az_el`, `gs232b_get_az_offset`, `gs232b_get_el`, `gs232b_get_el_offset`, `gs232b_get_state`, `gs232b_getdata`, `gs232b_goto`, `gs232b_send`, `gs232b_set_sensitivity`, `gs232b_state`, `gs232b_stop`, `gs232b_test`


### `gs232b_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `ic9100_lib.cpp`

**`device/general/ic9100_lib.h`** — 34 used / 34 exported

*Used:* `ic9100_byte`, `ic9100_check_address`, `ic9100_connect`, `ic9100_disconnect`, `ic9100_freq2band`, `ic9100_get_alcmeter`, `ic9100_get_bandpass`, `ic9100_get_bps9600mode`, `ic9100_get_compmeter`, `ic9100_get_datamode`, `ic9100_get_freqband`, `ic9100_get_frequency`, `ic9100_get_mode`, `ic9100_get_repeater_squelch`, `ic9100_get_rfgain`, `ic9100_get_rfmeter`, `ic9100_get_rfpower`, `ic9100_get_smeter`, `ic9100_get_squelch`, `ic9100_get_swrmeter`, `ic9100_handle`, `ic9100_set_bandpass`, `ic9100_set_bps9600mode`, `ic9100_set_channel`, `ic9100_set_datamode`, `ic9100_set_freqband`, `ic9100_set_frequency`, `ic9100_set_mode`, `ic9100_set_repeater_squelch`, `ic9100_set_rfgain` … (+4 more)

**`support/jsondef.h`** — 3 used / 195 exported

*Used:* `c_str`, `fmod`, `size`


### `ic9100_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/serial/serialclass.h`** — 1 used / 34 exported

*Used:* `Serial`


### `kisslib.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/kisslib.h`** — 31 used / 33 exported

*Used:* `KissHandle`, `get_command`, `get_control`, `get_data`, `get_destination_callsign`, `get_destination_stationID`, `get_error`, `get_port_number`, `get_protocolID`, `get_raw_packet`, `get_slip_packet`, `get_source_callsign`, `get_source_stationID`, `kissDecode`, `kissEncode`, `kissInspect`, `load_packet`, `print_ascii`, `print_hex`, `set_command`, `set_control`, `set_data`, `set_destination_callsign`, `set_destination_stationID`, `set_port_number`, `set_protocolID`, `set_raw_packet`, `set_slip_packet`, `set_source_callsign`, `set_source_stationID` … (+1 more)


### `kisslib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/sliplib.h`** — 0 used / 6 exported

*Exported but none matched:* `slip_calc_crc`, `slip_decode`, `slip_encode`, `slip_extract`, `slip_pack`, `slip_unpack`


### `kisstnc_lib.cpp`

**`device/general/kisstnc_lib.h`** — 10 used / 10 exported

*Used:* `KISSTNC_FULL_SIZE`, `kisstnc_calc_fcs`, `kisstnc_connect`, `kisstnc_disconnect`, `kisstnc_enterkiss`, `kisstnc_exitkiss`, `kisstnc_loadframe`, `kisstnc_recvframe`, `kisstnc_sendframe`, `kisstnc_unloadframe`


### `kisstnc_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `kpc9612p_lib.cpp`

**`device/general/kpc9612p_lib.h`** — 12 used / 13 exported

*Used:* `KPC9612P_FULL_SIZE`, `kpc9612p_calc_fcs`, `kpc9612p_connect`, `kpc9612p_disconnect`, `kpc9612p_entercmd`, `kpc9612p_enterkiss`, `kpc9612p_exitkiss`, `kpc9612p_handle`, `kpc9612p_loadframe`, `kpc9612p_recvframe`, `kpc9612p_sendframe`, `kpc9612p_unloadframe`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `secondsleep`


### `kpc9612p_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `mixwtnc_lib.cpp`

**`device/general/mixwtnc_lib.h`** — 10 used / 10 exported

*Used:* `MIXWTNC_FULL_SIZE`, `mixwtnc_calc_fcs`, `mixwtnc_connect`, `mixwtnc_disconnect`, `mixwtnc_enterkiss`, `mixwtnc_exitkiss`, `mixwtnc_loadframe`, `mixwtnc_recvframe`, `mixwtnc_sendframe`, `mixwtnc_unloadframe`


### `mixwtnc_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `pic_lib.cpp`

**`device/general/pic_lib.h`** — 8 used / 13 exported

*Used:* `PIC_HEADER_SIZE`, `pic_connect`, `pic_disconnect`, `pic_handle`, `suchi_camera`, `suchi_heater`, `suchi_report`, `suchi_shutter`

**`math/crclib.h`** — 0 used / 11 exported

*Exported but none matched:* `CRC16`, `calc`, `calc_crc16`, `calc_crc16_lsb`, `calc_crc16_msb`, `calc_crc16ccitt`, `calc_crc16ccitt_lsb`, `calc_crc16ccitt_msb`, `calc_crc16ibm`, `calc_file`, `crcset`


### `pic_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `prkx2su_class.cpp`

**`prkx2su_class.h`** — 20 used / 21 exported

*Used:* `Prkx2su`, `connect`, `disconnect`, `get_az`, `get_az_el`, `get_az_offset`, `get_el`, `get_el_offset`, `get_limits`, `get_status`, `getdata`, `gotoazel`, `maximum_speed`, `minimum_speed`, `ramp`, `send`, `set_sensitivity`, `stop`, `test`, `write_calibration`


### `prkx2su_class.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`device/serial/serialclass.h`** — 1 used / 34 exported

*Used:* `Serial`


### `prkx2su_lib.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/prkx2su_lib.h`** — 20 used / 21 exported

*Used:* `prkx2su_connect`, `prkx2su_disconnect`, `prkx2su_get_az`, `prkx2su_get_az_el`, `prkx2su_get_az_offset`, `prkx2su_get_el`, `prkx2su_get_el_offset`, `prkx2su_get_limits`, `prkx2su_get_state`, `prkx2su_getdata`, `prkx2su_goto`, `prkx2su_init`, `prkx2su_maximum_speed`, `prkx2su_minimum_speed`, `prkx2su_ramp`, `prkx2su_send`, `prkx2su_set_sensitivity`, `prkx2su_state`, `prkx2su_stop`, `prkx2su_test`


### `prkx2su_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`device/serial/serialclass.h`** — 0 used / 34 exported

*Exported but none matched:* `ReceiveBuffer`, `ReceiveByte`, `SendBuffer`, `SendByte`, `Serial`, `change_baud`, `close_device`, `defined`, `drain`, `get_char`, `get_cts`, `get_data`, `get_error`, `get_nmea`, `get_open`, `get_slip`, `get_string`, `get_xmodem`, `open_device`, `poll_char` … (+14 more)


### `ts2000_lib.cpp`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `defined`

**`device/general/ts2000_lib.h`** — 4 used / 4 exported

*Used:* `ts2000_connect`, `ts2000_disconnect`, `ts2000_get_frequency`, `ts2000_set_frequency`


### `ts2000_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`device/general/cssl_lib.h`** — 0 used / 24 exported

*Exported but none matched:* `_DCB`, `__cssl_t`, `cssl_close`, `cssl_drain`, `cssl_getchar`, `cssl_getdata`, `cssl_geterror`, `cssl_geterrormsg`, `cssl_getnmea`, `cssl_getslip`, `cssl_getxmodem`, `cssl_open`, `cssl_putchar`, `cssl_putdata`, `cssl_putnmea`, `cssl_putslip`, `cssl_putstring`, `cssl_setflowcontrol`, `cssl_settimeout`, `cssl_setup` … (+4 more)


### `unixgpio.cpp`

**`unixgpio.h`** — 9 used / 9 exported

*Used:* `Direction`, `UnixGpio`, `Value`, `geterror`, `getpath`, `isopen`, `read`, `setDirection`, `write`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `data_isfile`


### `unixgpio.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `usrp_lib.cpp`

**`device/general/usrp_lib.h`** — 10 used / 33 exported

*Used:* `usrp_byte`, `usrp_connect`, `usrp_disconnect`, `usrp_freq2band`, `usrp_get_frequency`, `usrp_get_record`, `usrp_handle`, `usrp_send`, `usrp_set_frequency`, `usrp_set_record`

**`support/jsondef.h`** — 1 used / 195 exported

*Used:* `c_str`


### `usrp_lib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/socketlib.h`** — 1 used / 37 exported

*Used:* `socket_channel`

**`support/stringlib.h`** — 0 used / 44 exported

*Exported but none matched:* `StringParser`, `byte_vector_to_string`, `clean_string`, `from_hex`, `from_hex_string`, `from_hex_vector`, `getFieldNumber`, `getFieldNumberAsDouble`, `getFieldNumberAsInteger`, `splitString`, `string_cmp`, `string_find`, `string_join`, `string_parse`, `string_replace`, `string_split`, `string_to_byte_vector`, `to_angle`, `to_astring`, `to_binary` … (+24 more)


## device/i2c

### `i2c.cpp`

**`device/i2c/i2c.h`** — 12 used / 15 exported

*Used:* `I2C`, `communicate`, `connect`, `defined`, `get_connected`, `get_error`, `get_fh`, `poll`, `receive`, `send`, `set_address`, `set_delay`


### `i2c.h`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`support/elapsedtime.h`** — 0 used / 16 exported

*Exported but none matched:* `ElapsedTime`, `check`, `getElapsedTime`, `getElapsedTimeMiliSeconds`, `getElapsedTimeSince`, `info`, `lap`, `printElapsedTime`, `reset`, `split`, `start`, `stop`, `tic`, `timed_countdown`, `timer`, `toc`

**`device/i2c/i2c-dev-smbus.h`** — 1 used / 25 exported

*Used:* `defined`


## device/netradio

### `netradio.cpp`

**`netradio.h`** — 14 used / 20 exported

*Used:* `Clear`, `DeQueue`, `Init`, `NET_DOWN_PORT`, `NET_UP_PORT`, `NetRadio`, `PacketIn`, `PacketInSize`, `PacketOut`, `PacketOutSize`, `Packetize`, `Queue`, `UnPacketize`, `queue_loop`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `secondsleep`

**`support/elapsedtime.h`** — 1 used / 16 exported

*Used:* `ElapsedTime`


### `netradio.h`

**`support/packetcomm.h`** — 1 used / 22 exported

*Used:* `PacketComm`

**`support/socketlib.h`** — 2 used / 37 exported

*Used:* `socket_bus`, `socket_channel`


## device/serial

### `serialclass.cpp`

**`support/configCosmos.h`** — 2 used / 12 exported

*Used:* `close`, `defined`

**`device/serial/serialclass.h`** — 33 used / 34 exported

*Used:* `ReceiveBuffer`, `ReceiveByte`, `SendBuffer`, `SendByte`, `Serial`, `change_baud`, `close_device`, `defined`, `drain`, `get_char`, `get_cts`, `get_data`, `get_error`, `get_nmea`, `get_open`, `get_slip`, `get_string`, `get_xmodem`, `open_device`, `poll_char`, `put_char`, `put_data`, `put_nmea`, `put_slip`, `put_string`, `set_dtr`, `set_flowcontrol`, `set_params`, `set_restoreonclose`, `set_rtimeout` … (+3 more)


### `serialclass.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `defined`

**`support/sliplib.h`** — 0 used / 6 exported

*Exported but none matched:* `slip_calc_crc`, `slip_decode`, `slip_encode`, `slip_extract`, `slip_pack`, `slip_unpack`

**`support/elapsedtime.h`** — 1 used / 16 exported

*Used:* `stop`


## math

### `bytelib.cpp`

**`bytelib.h`** — 16 used / 17 exported

*Used:* `doublefrom`, `doubleto`, `floatfrom`, `floatto`, `int16from`, `int16to`, `int32from`, `int32to`, `local_byte_order`, `uint16from`, `uint16to`, `uint32from`, `uint32to`, `uint8from`, `uint8to`, `uintswap`


### `bytelib.h`

**`math/constants.h`** — 1 used / 5 exported

*Used:* `ByteOrder`


### `constants.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`


### `crclib.cpp`

**`crclib.h`** — 10 used / 11 exported

*Used:* `CRC16`, `calc`, `calc_crc16`, `calc_crc16_lsb`, `calc_crc16_msb`, `calc_crc16ccitt`, `calc_crc16ccitt_lsb`, `calc_crc16ccitt_msb`, `calc_crc16ibm`, `calc_file`


### `crclib.h`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`

**`math/bytelib.h`** — 0 used / 17 exported

*Exported but none matched:* `doublefrom`, `doubleto`, `floatfrom`, `floatto`, `int16from`, `int16to`, `int32from`, `int32to`, `local_byte_order`, `uint16from`, `uint16to`, `uint32from`, `uint32to`, `uint8from`, `uint8to`, `uintswap`, `vector8invert`


### `mathlib.cpp`

**`math/mathlib.h`** — 77 used / 78 exported

*Used:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope`, `firstx`, `fit`, `fitelement`, `fixangle`, `fixprecision`, `gauss_jackson_dekernel`, `gauss_jackson_destep`, `gauss_jackson_extrapolate`, `gauss_jackson_getstep`, `gauss_jackson_instance` … (+47 more)


### `mathlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`vector.h`** — 12 used / 148 exported

*Used:* `avector`, `cvector`, `drotate`, `gvector`, `irotate`, `qcomplex`, `qfirst`, `qlast`, `quaternion`, `rvector`, `size`, `svector`

**`matrix.h`** — 2 used / 105 exported

*Used:* `matrix1d`, `size`

**`rotation.h`** — 0 used / 11 exported

*Exported but none matched:* `DCM`, `base1_from_base2`, `base2_from_base1`, `basisOrthonormal`, `cm_quaternion2dcm`, `dotProduct`, `normalize`, `q_dcm2quaternion_cm`, `q_dcm2quaternion_rm`, `rm_quaternion2dcm`, `transposeMatrix`


### `matrix.cpp`

**`matrix.h`** — 88 used / 105 exported

*Used:* `Matrix`, `Vector`, `cm3x3_to_m2`, `cm_add`, `cm_change_around`, `cm_change_around_x`, `cm_change_around_y`, `cm_change_around_z`, `cm_diag`, `cm_eye`, `cm_from_rm`, `cm_mmult`, `cm_mult`, `cm_smult`, `cm_square`, `cm_sub`, `cm_transpose`, `cm_zero`, `cmatrix`, `cv_diag`, `cv_mmult`, `cv_to_m1`, `cv_to_m2`, `determinant`, `determinant_rm`, `diag`, `eye`, `inverse`, `m1_add`, `m1_cross` … (+58 more)


### `matrix.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/vector.h`** — 14 used / 148 exported

*Used:* `Quaternion`, `Vector`, `array_items`, `cvector`, `dump`, `empty`, `eye`, `from_json`, `is_null`, `normalize`, `parse`, `rvector`, `size`, `to_json`


### `rotation.cpp`

**`rotation.h`** — 11 used / 11 exported

*Used:* `DCM`, `base1_from_base2`, `base2_from_base1`, `basisOrthonormal`, `cm_quaternion2dcm`, `dotProduct`, `normalize`, `q_dcm2quaternion_cm`, `q_dcm2quaternion_rm`, `rm_quaternion2dcm`, `transposeMatrix`


### `rotation.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`vector.h`** — 3 used / 148 exported

*Used:* `cvector`, `normalize`, `quaternion`

**`matrix.h`** — 3 used / 105 exported

*Used:* `cmatrix`, `normalize`, `rmatrix`


### `vector.cpp`

**`vector.h`** — 126 used / 148 exported

*Used:* `Quaternion`, `Quaternion2quaternion`, `Vector`, `a_quaternion2euler`, `area`, `avector`, `clear`, `conjugate`, `cross`, `cv_add`, `cv_cross`, `cv_div`, `cv_mult`, `cv_norm`, `cv_normal`, `cv_one`, `cv_quaternion2axis`, `cv_sadd`, `cv_smult`, `cv_sqrt`, `cv_sub`, `cv_unitx`, `cv_unity`, `cv_unitz`, `cv_zero`, `cvector`, `dot`, `dot_cv`, `dot_rv`, `drotate` … (+96 more)

**`matrix.h`** — 4 used / 105 exported

*Used:* `Vector`, `eye`, `normalize`, `size`


### `vector.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`constants.h`** — 0 used / 3 exported

*Exported but none matched:* `Constants`, `massEarth`, `radiusEarth`


## module

### `file_module.cpp`

**`file_module.h`** — 6 used / 10 exported

*Used:* `FileModule`, `Init`, `Loop`, `shutdown`, `soft_shutdown`, `transfer`


### `file_module.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`agent/agentclass.h`** — 3 used / 164 exported

*Used:* `Agent`, `send`, `shutdown`

**`support/packetcomm.h`** — 1 used / 22 exported

*Used:* `PacketComm`

**`support/transferclass.h`** — 5 used / 44 exported

*Used:* `Init`, `SendRetVal`, `Sender`, `Transfer`, `send`


### `node_propagator_module.cpp`

**`node_propagator_module.h`** — 2 used / 2 exported

*Used:* `Init`, `NodePropagatorModule`


### `node_propagator_module.h`

**`physics/simulatorclass.h`** — 2 used / 38 exported

*Used:* `Init`, `Simulator`


### `packethandler_module.cpp`

**`packethandler_module.h`** — 4 used / 4 exported

*Used:* `Init`, `Loop`, `PacketHandlerModule`, `shutdown`


### `packethandler_module.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`agent/agentclass.h`** — 3 used / 164 exported

*Used:* `Agent`, `channel_name`, `shutdown`

**`support/packethandler.h`** — 1 used / 71 exported

*Used:* `PacketHandler`


### `websocket_module.cpp`

**`websocket_module.h`** — 12 used / 12 exported

*Used:* `Init`, `Loop`, `PacketizeFunction`, `Receive`, `Transmit`, `WebsocketModule`, `packetize_function`, `set_AX25_Packetize_args`, `set_packetize_function`, `set_unpacketize_function`, `shutdown`, `unpacketize_function`


### `websocket_module.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`agent/agentclass.h`** — 3 used / 164 exported

*Used:* `Agent`, `channel_name`, `shutdown`

**`support/socketlib.h`** — 1 used / 37 exported

*Used:* `socket_channel`


## physics

### `constants.cpp`

**`physics/constants.h`** — 3 used / 3 exported

*Used:* `Constants`, `massEarth`, `radiusEarth`


### `constants.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `controllib.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`physics/controllib.h`** — 1 used / 1 exported

*Used:* `calc_control_torque`


### `controllib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/convertdef.h`** — 1 used / 34 exported

*Used:* `qatt`


### `nrlmsise-00.cpp`

**`physics/nrlmsise-00.h`** — 8 used / 8 exported

*Used:* `ap_array`, `ghp7`, `gtd7`, `gtd7d`, `gts7`, `nrlmsise_flags`, `nrlmsise_input`, `nrlmsise_output`


### `nrlmsise-00.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `physicsclass.cpp`

**`physicsclass.h`** — 64 used / 72 exported

*Used:* `AttAccel`, `ControlAccel`, `ControlThrust`, `ControlTorque`, `Converge`, `ElectricalPropagator`, `End`, `ExternalPanelType`, `GaussJacksonPositionPropagator`, `GeoAttitudePropagator`, `GeoPositionPropagator`, `GravityAccel`, `GravityParams`, `InertialAttitudePropagator`, `InertialPositionPropagator`, `Init`, `IterativeAttitudePropagator`, `IterativePositionPropagator`, `LunarPosAccel`, `LunarPositionPropagator`, `LvlhAttitudePropagator`, `LvlhPositionPropagator`, `MetricGenerator`, `Msis00Density`, `Nplgndr`, `OrbitalEventGenerator`, `PhysCalc`, `PhysSetup`, `PosAccel`, `Propagate` … (+34 more)

**`support/enumlib.h`** — 4 used / 5 exported

*Used:* `Enum`, `Exists`, `Init`, `Size`


### `physicsclass.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jsonlib.h`** — 1 used / 204 exported

*Used:* `json_init`

**`support/convertlib.h`** — 2 used / 157 exported

*Used:* `Vector`, `shape2eci`

**`support/elapsedtime.h`** — 1 used / 16 exported

*Used:* `ElapsedTime`

**`physics/nrlmsise-00.h`** — 0 used / 8 exported

*Exported but none matched:* `ap_array`, `ghp7`, `gtd7`, `gtd7d`, `gts7`, `nrlmsise_flags`, `nrlmsise_input`, `nrlmsise_output`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `Init`

**`support/demlib.h`** — 0 used / 9 exported

*Exported but none matched:* `map_dem_alt`, `map_dem_cache`, `map_dem_close`, `map_dem_init`, `map_dem_open`, `map_dem_pixel`, `map_dem_scale`, `map_dem_tilt`, `planet_dem`


### `physicsdef.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/vector.h`** — 2 used / 148 exported

*Used:* `area`, `rvector`

**`support/convertdef.h`** — 1 used / 34 exported

*Used:* `locstruc`


### `physicslib.cpp`

**`physics/physicslib.h`** — 31 used / 36 exported

*Used:* `att_accel`, `gauss_jackson_converge_hardware`, `gauss_jackson_converge_orbit`, `gauss_jackson_init`, `gauss_jackson_init_eci`, `gauss_jackson_init_stk`, `gauss_jackson_init_tle`, `gauss_jackson_propagate`, `gauss_jackson_setup`, `gauss_jackson_update_eci`, `gravity_accel`, `gravity_accel2`, `gravity_params`, `gravity_potential`, `gravity_vector`, `groundstation`, `hardware_init_eci`, `initialize_imu`, `msis00_density`, `nplgndr`, `orbit_init`, `orbit_init_eci`, `orbit_init_shape`, `orbit_init_tle`, `orbit_propagate`, `pos_accel`, `propagate`, `rearth`, `simulate_hardware`, `simulate_imu` … (+1 more)

**`support/jsondef.h`** — 13 used / 195 exported

*Used:* `Vector`, `c_str`, `capacity`, `cos`, `cosmosstruc`, `devspecstruc`, `physicsstruc`, `pow`, `push_back`, `sin`, `size`, `sqrt`, `units`


### `physicslib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`physics/nrlmsise-00.h`** — 0 used / 8 exported

*Exported but none matched:* `ap_array`, `ghp7`, `gtd7`, `gtd7d`, `gts7`, `nrlmsise_flags`, `nrlmsise_input`, `nrlmsise_output`

**`support/convertlib.h`** — 0 used / 157 exported

*Exported but none matched:* `Position`, `Rearth`, `Vector`, `atan3`, `att_clear`, `att_extra`, `att_geoc`, `att_geoc2icrf`, `att_icrf`, `att_icrf2geoc`, `att_icrf2lvlh`, `att_icrf2selc`, `att_lvlh`, `att_lvlh2icrf`, `att_lvlh2planec`, `att_planec2lvlh`, `att_planec2topo`, `att_selc`, `att_selc2icrf`, `att_set_geoc` … (+137 more)

**`support/demlib.h`** — 0 used / 9 exported

*Exported but none matched:* `map_dem_alt`, `map_dem_cache`, `map_dem_close`, `map_dem_init`, `map_dem_open`, `map_dem_pixel`, `map_dem_scale`, `map_dem_tilt`, `planet_dem`

**`physics/physicsdef.h`** — 1 used / 6 exported

*Used:* `gj_handle`

**`support/jsondef.h`** — 3 used / 195 exported

*Used:* `cosmosstruc`, `devspecstruc`, `physicsstruc`


### `simulatorclass.cpp`

**`simulatorclass.h`** — 36 used / 38 exported

*Used:* `AddDetector`, `AddNode`, `AddTarget`, `End`, `Formation`, `GetEnd`, `GetError`, `GetNode`, `GetNodes`, `GetState`, `Init`, `Metric`, `NudgeNode`, `ParseOrbitFile`, `ParseOrbitString`, `ParseSatFile`, `ParseSatString`, `ParseTargetFile`, `ParseTargetJson`, `ParseTargetString`, `Pause`, `Propagate`, `Reset`, `Run`, `Simulator`, `State`, `StateList`, `Target`, `Thrust`, `UpdatePush` … (+6 more)

**`support/jsonclass.h`** — 2 used / 20 exported

*Used:* `Json`, `Type`

**`support/stringlib.h`** — 2 used / 44 exported

*Used:* `string_split`, `to_unsigned`


### `simulatorclass.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jsondef.h`** — 8 used / 195 exported

*Used:* `Vector`, `camstruc`, `clear`, `cosmosstruc`, `empty`, `name`, `size`, `targetstruc`

**`support/convertlib.h`** — 1 used / 157 exported

*Used:* `Vector`

**`physics/physicsclass.h`** — 11 used / 72 exported

*Used:* `End`, `Init`, `Propagate`, `Propagator`, `Reset`, `State`, `Type`, `Update`, `Vector`, `propagation_priority`, `qatt`


## support

### `ax25class.cpp`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`

**`support/ax25class.h`** — 25 used / 28 exported

*Used:* `Ax25Handle`, `get_ax25_packet`, `get_control`, `get_data`, `get_destination_callsign`, `get_destination_stationID`, `get_hdlc_packet`, `get_header`, `get_protocolID`, `get_source_callsign`, `get_source_stationID`, `load`, `packet_header`, `set_ax25_packet`, `set_control`, `set_data`, `set_destination_callsign`, `set_destination_stationID`, `set_hdlc_packet`, `set_protocolID`, `set_source_callsign`, `set_source_stationID`, `stuff`, `unload`, `unstuff`


### `ax25class.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`

**`math/crclib.h`** — 1 used / 11 exported

*Used:* `CRC16`


### `beacon.cpp`

**`beacon.h`** — 22 used / 29 exported

*Used:* `Beacon`, `Decode`, `EncodeBinary`, `EncodeJson`, `Init`, `TypeId`, `adcsgyro_count`, `adcsmtr_count`, `adcsrw_count`, `add_beacon`, `camera_count`, `cpu_count`, `epsbatt_count`, `epsbcreg_count`, `epsswch_count`, `get_interval`, `large_beacon`, `ltsen_count`, `rxrtxr_count`, `set_pattern`, `target_count`, `tsen_count`

**`support/timelib.h`** — 5 used / 61 exported

*Used:* `currentmjd`, `decisec`, `decisec2mjd`, `unix2utc`, `utc2unixseconds`


### `beacon.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `close`

**`support/jsonlib.h`** — 0 used / 204 exported

*Exported but none matched:* `add_node_id`, `calc_events`, `change_node_id`, `check_node_id`, `create_databases`, `device_component_index`, `device_has_property`, `device_index`, `device_type_index`, `device_type_name`, `json_adddevice`, `json_addentry`, `json_addpiece`, `json_append`, `json_checkentry`, `json_clear_cosmosstruc`, `json_clone_node`, `json_convert_double`, `json_convert_float`, `json_convert_int16` … (+184 more)

**`math/mathlib.h`** — 2 used / 78 exported

*Used:* `size`, `update`

**`support/enumlib.h`** — 1 used / 5 exported

*Used:* `Init`

**`support/packetcomm.h`** — 3 used / 22 exported

*Used:* `PacketComm`, `TypeId`, `__attribute__`


### `channellib.cpp`

**`channellib.h`** — 27 used / 33 exported

*Used:* `Add`, `Age`, `ByteRate`, `Bytes`, `Channel`, `Check`, `Clear`, `Enable`, `Enabled`, `Find`, `Increment`, `Init`, `Level`, `Packets`, `Pull`, `Push`, `Size`, `TestLoop`, `TestRemaining`, `TestSpeed`, `TestStart`, `TestStop`, `Touch`, `Update`, `WakeupTimer`, `channelstruc`, `start`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `secondsleep`


### `channellib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/packetcomm.h`** — 1 used / 22 exported

*Used:* `PacketComm`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `get_unix_time`

**`support/elapsedtime.h`** — 3 used / 16 exported

*Used:* `ElapsedTime`, `start`, `stop`


### `check.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/check.h`** — 11 used / 17 exported

*Used:* `Checker`, `Report`, `Save`, `Test`, `dump`, `empty`, `from_json`, `record`, `resize`, `size`, `to_json`

**`support/timelib.h`** — 2 used / 61 exported

*Used:* `currentmjd`, `utc2datetime`


### `check.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `configCosmos.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`

**`support/cosmos-errclass.h`** — 0 used / 3 exported

*Exported but none matched:* `Error`, `ErrorString`, `cosmos_error_string`


### `configCosmosKernel.h`

**`support/cosmos-defs.h`** — 0 used / 3 exported

*Exported but none matched:* `NODE_FLAG`, `NODE_TYPE`, `PIECE`


### `convertdef.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 1 used / 78 exported

*Used:* `size`


### `convertlib.cpp`

**`support/convertlib.h`** — 147 used / 157 exported

*Used:* `Rearth`, `Vector`, `atan3`, `att_clear`, `att_extra`, `att_geoc`, `att_geoc2icrf`, `att_icrf`, `att_icrf2geoc`, `att_icrf2lvlh`, `att_icrf2selc`, `att_lvlh`, `att_lvlh2icrf`, `att_lvlh2planec`, `att_planec2lvlh`, `att_planec2topo`, `att_selc`, `att_selc2icrf`, `att_set_geoc`, `att_set_icrf`, `att_set_lvlh`, `att_set_selc`, `att_set_topo`, `att_topo`, `att_topo2planec`, `body2topo`, `cart2peri`, `eci2hill`, `eci2kep`, `eci2lvlh` … (+117 more)

**`support/jsondef.h`** — 11 used / 195 exported

*Used:* `Vector`, `c_str`, `cos`, `file`, `fmod`, `name`, `pow`, `push_back`, `sin`, `size`, `sqrt`

**`support/stringlib.h`** — 3 used / 44 exported

*Used:* `to_fixed`, `to_floating`, `to_unsigned`


### `convertlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`support/convertdef.h`** — 10 used / 34 exported

*Used:* `attstruc`, `cartpos`, `geoidpos`, `kepstruc`, `locstruc`, `sgp4struc`, `spherpos`, `stkstruc`, `tlestruc`, `utc`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`support/geomag.h`** — 0 used / 1 exported

*Exported but none matched:* `geomag_front`

**`support/ephemlib.h`** — 0 used / 4 exported

*Exported but none matched:* `jpllib`, `jplnut`, `jplopen`, `jplpos`


### `cosmos-errclass.cpp`

**`support/cosmos-errclass.h`** — 3 used / 3 exported

*Used:* `Error`, `ErrorString`, `cosmos_error_string`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)


### `cosmos-errclass.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`


### `cosmos-errno.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`


### `datalib.cpp`

**`support/datalib.h`** — 61 used / 62 exported

*Used:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive`, `data_list_archive_days`, `data_list_files`, `data_list_latest_file`, `data_list_nodes`, `data_load_archive`, `data_move_file`, `data_name`, `data_name_date`, `data_name_path`, `data_name_struc` … (+31 more)

**`support/elapsedtime.h`** — 3 used / 16 exported

*Used:* `ElapsedTime`, `split`, `timer`

**`support/timelib.h`** — 6 used / 61 exported

*Used:* `cal2mjd`, `currentmjd`, `mjd2ymd`, `timeval`, `unix2utc`, `utc2unix`

**`support/stringlib.h`** — 2 used / 44 exported

*Used:* `string_replace`, `string_split`


### `datalib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`thirdparty/zlib/zlib.h`** — header not indexed (third-party or missing)


### `demlib.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/demlib.h`** — 8 used / 9 exported

*Used:* `map_dem_alt`, `map_dem_cache`, `map_dem_init`, `map_dem_open`, `map_dem_pixel`, `map_dem_scale`, `map_dem_tilt`, `planet_dem`

**`math/mathlib.h`** — 1 used / 78 exported

*Used:* `size`


### `demlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`support/datalib.h`** — 0 used / 62 exported

*Exported but none matched:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive` … (+42 more)


### `elapsedtime.cpp`

**`support/elapsedtime.h`** — 14 used / 16 exported

*Used:* `ElapsedTime`, `getElapsedTime`, `getElapsedTimeSince`, `info`, `lap`, `printElapsedTime`, `reset`, `split`, `start`, `stop`, `tic`, `timed_countdown`, `timer`, `toc`

**`support/timelib.h`** — 2 used / 61 exported

*Used:* `currentmjd`, `secondsleep`


### `elapsedtime.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `enumlib.cpp`

**`support/convertlib.h`** — 0 used / 157 exported

*Exported but none matched:* `Position`, `Rearth`, `Vector`, `atan3`, `att_clear`, `att_extra`, `att_geoc`, `att_geoc2icrf`, `att_icrf`, `att_icrf2geoc`, `att_icrf2lvlh`, `att_icrf2selc`, `att_lvlh`, `att_lvlh2icrf`, `att_lvlh2planec`, `att_planec2lvlh`, `att_planec2topo`, `att_selc`, `att_selc2icrf`, `att_set_geoc` … (+137 more)

**`enumlib.h`** — 5 used / 5 exported

*Used:* `Enum`, `Exists`, `Extend`, `Init`, `Size`


### `envi.cpp`

**`envi.h`** — 7 used / 7 exported

*Used:* `MapInfoGeoLatLon`, `envi_hdr`, `read_envi_data`, `read_envi_hdr`, `to_string`, `write_envi_data`, `write_envi_hdr`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `data_isfile`

**`math/bytelib.h`** — 1 used / 17 exported

*Used:* `local_byte_order`


### `envi.h`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`


### `ephemlib.cpp`

**`support/ephemlib.h`** — 4 used / 4 exported

*Used:* `jpllib`, `jplnut`, `jplopen`, `jplpos`

**`support/jpleph.h`** — 2 used / 11 exported

*Used:* `jpl_init_ephemeris`, `jpl_pleph`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `get_cosmosresources`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)


### `ephemlib.h`

**`support/convertdef.h`** — 2 used / 34 exported

*Used:* `cartpos`, `utc`


### `geomag.cpp`

**`support/geomag.h`** — 1 used / 1 exported

*Used:* `geomag_front`


### `geomag.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`support/datalib.h`** — 0 used / 62 exported

*Exported but none matched:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive` … (+42 more)


### `jpleph.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jpleph.h`** — 9 used / 11 exported

*Used:* `JPL_HEADER_SIZE`, `interpolation_info`, `jpl_close_ephemeris`, `jpl_eph_data`, `jpl_get_double`, `jpl_get_long`, `jpl_init_ephemeris`, `jpl_pleph`, `jpl_state`


### `jpleph.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `jsonclass.cpp`

**`jsonclass.h`** — 19 used / 20 exported

*Used:* `Array`, `Json`, `Type`, `Value`, `contents_array`, `contents_bool`, `contents_number`, `contents_object`, `contents_string`, `contents_type`, `extract_contents`, `extract_member`, `extract_members`, `extract_name`, `extract_value`, `extract_values`, `skip_character`, `skip_to_character`, `skip_white`


### `jsonclass.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `jsondef.cpp`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`

**`support/jsondef.h`** — 29 used / 195 exported

*Used:* `DBSchema`, `add_default_names`, `add_name`, `agent_name`, `back`, `capacity`, `clear`, `column_names`, `cosmos2table`, `cosmosstruc`, `find_table`, `get_type`, `get_value`, `init`, `init_database`, `insert`, `insert_statement`, `insert_type`, `insert_value`, `length`, `multi_table_insert_statement`, `name`, `namespace_names`, `node_name`, `push_back`, `schema_name`, `size`, `table_insert_statement`, `table_name`


### `jsondef.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `close`

**`math/mathlib.h`** — 2 used / 78 exported

*Used:* `accel`, `size`

**`support/convertdef.h`** — 21 used / 34 exported

*Used:* `attstruc`, `cartpos`, `dump`, `empty`, `extraatt`, `extrapos`, `from_json`, `geoidpos`, `is_null`, `locstruc`, `long_value`, `memoryusage`, `number_value`, `parse`, `posstruc`, `qatt`, `spherpos`, `string_value`, `tlestruc`, `to_json`, `utc`

**`physics/physicsdef.h`** — 1 used / 6 exported

*Used:* `coverage`

**`support/socketlib.h`** — 5 used / 37 exported

*Used:* `NetworkType`, `close`, `sockaddr_in`, `sockaddr_in6`, `socket_channel`

**`support/objlib.h`** — 17 used / 43 exported

*Used:* `begin`, `dump`, `empty`, `end`, `face`, `from_json`, `group`, `is_null`, `line`, `material`, `number_value`, `parse`, `point`, `size`, `string_value`, `to_json`, `wavefront`


### `jsonlib.cpp`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`

**`support/jsonlib.h`** — 74 used / 204 exported

*Used:* `json_adddevice`, `json_addentry`, `json_addpiece`, `json_append`, `json_checkentry`, `json_count_hash`, `json_count_total`, `json_create_node`, `json_createpiece`, `json_createport`, `json_destroy`, `json_entry_of`, `json_equation`, `json_equation_map`, `json_extract_string`, `json_findcomp`, `json_finddev`, `json_findpiece`, `json_hash`, `json_init`, `json_init_device_type_string`, `json_init_node`, `json_init_reserve`, `json_init_unit`, `json_mapcompentry`, `json_mapdeviceentry`, `json_mappieceentry`, `json_mapportentry`, `json_name_map`, `json_out_attstruc` … (+44 more)

**`support/convertlib.h`** — 1 used / 157 exported

*Used:* `Vector`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `currentmjd`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jsonclass.h`** — 0 used / 20 exported

*Exported but none matched:* `Array`, `Json`, `Type`, `Value`, `contents_array`, `contents_bool`, `contents_number`, `contents_object`, `contents_string`, `contents_type`, `extract_contents`, `extract_member`, `extract_members`, `extract_name`, `extract_string`, `extract_value`, `extract_values`, `skip_character`, `skip_to_character`, `skip_white`


### `jsonlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/jsondef.h`** — 16 used / 195 exported

*Used:* `NODE_ID_TYPE`, `Vector`, `beatstruc`, `cosmosstruc`, `devicestruc`, `eventstruc`, `file`, `jsonentry`, `jsonequation`, `jsonhandle`, `jsonnode`, `jsonoperand`, `jsontoken`, `name`, `node_name`, `targetstruc`

**`support/stringlib.h`** — 0 used / 44 exported

*Exported but none matched:* `StringParser`, `byte_vector_to_string`, `clean_string`, `from_hex`, `from_hex_string`, `from_hex_vector`, `getFieldNumber`, `getFieldNumberAsDouble`, `getFieldNumberAsInteger`, `splitString`, `string_cmp`, `string_find`, `string_join`, `string_parse`, `string_replace`, `string_split`, `string_to_byte_vector`, `to_angle`, `to_astring`, `to_binary` … (+24 more)


### `jsonobject.cpp`

**`support/jsonobject.h`** — 7 used / 7 exported

*Used:* `JSONObject`, `addElement`, `clear`, `get_json_string`, `jsonTuple`, `to_json_object`, `to_json_string`


### `jsonobject.h`

**`support/jsonvalue.h`** — 4 used / 12 exported

*Used:* `JSONArray`, `JSONObject`, `JSONValue`, `to_json_string`


### `jsonvalue.cpp`

**`support/jsonvalue.h`** — 12 used / 12 exported

*Used:* `JSONArray`, `JSONObject`, `JSONValue`, `JsonValueType`, `isArray`, `isBool`, `isNull`, `isNumber`, `isObject`, `isString`, `set_null`, `to_json_string`

**`support/jsonobject.h`** — 3 used / 7 exported

*Used:* `JSONObject`, `addElement`, `to_json_string`

**`support/stringlib.h`** — 3 used / 44 exported

*Used:* `to_floatany`, `to_signed`, `to_unsigned`


### `jsonvalue.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/vector.h`** — 2 used / 148 exported

*Used:* `Quaternion`, `Vector`


### `logger.cpp`

**`support/stringlib.h`** — 1 used / 44 exported

*Used:* `to_unsigned`

**`support/datalib.h`** — 3 used / 62 exported

*Used:* `Init`, `data_base_path`, `data_name`

**`support/logger.h`** — 10 used / 11 exported

*Used:* `Close`, `Init`, `Level`, `Logger`, `Monitor`, `Open`, `Printf`, `Set`, `Type`, `log_msg`


### `logger.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/elapsedtime.h`** — 1 used / 16 exported

*Used:* `ElapsedTime`

**`support/timelib.h`** — 0 used / 61 exported

*Exported but none matched:* `DAYS_TO_SECONDS`, `DateTime`, `JD2MJD`, `MJD2JD`, `SECONDS_TO_DAYS`, `TIME_DAYS_TO_SECS`, `TIME_MJD_TO_UNIXs`, `TIME_SECS_SINCE_MJD`, `TIME_SECS_TO_DAYS`, `TIME_UNIX_TV_TO_DOUBLE_SECS`, `TIME_UNIXs_TO_MJD`, `UPTIME`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time` … (+41 more)

**`support/socketlib.h`** — 1 used / 37 exported

*Used:* `socket_bus`


### `objlib.cpp`

**`support/objlib.h`** — 30 used / 43 exported

*Used:* `add_1u`, `add_3u`, `add_cuboid`, `add_face`, `add_geometric_vertex`, `add_line`, `add_material`, `add_normal_vertex`, `add_parameter_vertex`, `add_point`, `add_texture_vertex`, `begin`, `face`, `group`, `line`, `load_file`, `material`, `modify_groups`, `parsemtl`, `parseobj`, `point`, `rebase`, `save_file`, `size`, `split`, `update_face`, `update_group`, `update_line`, `vertex`, `wavefront`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `data_isfile`


### `objlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 1 used / 78 exported

*Used:* `size`

**`math/vector.h`** — 12 used / 148 exported

*Used:* `Quaternion`, `Vector`, `area`, `dump`, `empty`, `from_json`, `is_null`, `length`, `number_value`, `parse`, `size`, `to_json`


### `packetcomm.cpp`

**`packetcomm.h`** — 18 used / 22 exported

*Used:* `ASMPacketize`, `ASMUnPacketize`, `AX25Packetize`, `AX25UnPacketize`, `CalcCRC`, `CheckCRC`, `HDLCPacketize`, `HDLCUnPacketize`, `Invert`, `PacketComm`, `PacketStyle`, `RawPacketize`, `RawUnPacketize`, `SLIPPacketize`, `SLIPUnPacketize`, `TypeId`, `Unwrap`, `Wrap`

**`math/constants.h`** — 1 used / 5 exported

*Used:* `ByteOrder`

**`math/bytelib.h`** — 3 used / 17 exported

*Used:* `uint16from`, `uint32to`, `uint8from`

**`support/sliplib.h`** — 2 used / 6 exported

*Used:* `slip_pack`, `slip_unpack`

**`support/ax25class.h`** — 8 used / 28 exported

*Used:* `Ax25Handle`, `get_data`, `get_hdlc_packet`, `load`, `set_ax25_packet`, `stuff`, `unload`, `unstuff`

**`support/timelib.h`** — 1 used / 61 exported

*Used:* `decisec`


### `packetcomm.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`

**`math/crclib.h`** — 1 used / 11 exported

*Used:* `CRC16`


### `packethandler.cpp`

**`packethandler.h`** — 62 used / 71 exported

*Used:* `AdcsForward`, `ClearQueue`, `CreateBeacon`, `DecodeAdcsResponse`, `DecodeBeacon`, `DecodeEpsResponse`, `DecodePong`, `DecodeResponse`, `DecodeTest`, `DecodeTime`, `EnableChannel`, `EpsForward`, `ExecForward`, `ExternalCommand`, `ExternalTask`, `FileForward`, `FuncEntry`, `GetTimeBinary`, `GetTimeHuman`, `Halt`, `Heartbeat`, `ImageForward`, `InternalRequest`, `ListDirectory`, `PacketHandler`, `Ping`, `QueueAdcsCommunicate`, `QueueAdcsFree`, `QueueAdcsInertial`, `QueueAdcsLvlh` … (+32 more)

**`beacon.h`** — 6 used / 29 exported

*Used:* `Beacon`, `Decode`, `EncodeBinary`, `EncodeJson`, `Init`, `TypeId`


### `packethandler.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/packetcomm.h`** — 2 used / 22 exported

*Used:* `PacketComm`, `TypeId`

**`support/beacon.h`** — 1 used / 29 exported

*Used:* `TypeId`

**`support/convertlib.h`** — 1 used / 157 exported

*Used:* `Vector`

**`agent/agentclass.h`** — 2 used / 164 exported

*Used:* `Agent`, `start`


### `print_utils.cpp`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/print_utils.h`** — 8 used / 9 exported

*Used:* `PrintUtils`, `endline`, `quat`, `reset`, `scalar`, `text`, `vector2`, `vectorAndMag`


### `print_utils.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)


### `sliplib.cpp`

**`support/sliplib.h`** — 6 used / 6 exported

*Used:* `slip_calc_crc`, `slip_decode`, `slip_encode`, `slip_extract`, `slip_pack`, `slip_unpack`

**`support/cosmos-errno.h`** — 0 used / 1 exported

*Exported but none matched:* `ErrorNumbers`


### `sliplib.h`

**`support/configCosmosKernel.h`** — 0 used / 1 exported

*Exported but none matched:* `COSMOS_SIZEOF`


### `socketlib.cpp`

**`support/socketlib.h`** — 33 used / 37 exported

*Used:* `NetworkType`, `Udp`, `close`, `errorStatus`, `hostnameToIP`, `receiveLoop`, `receiveOnce`, `send`, `setupClient`, `setupClientAcstb`, `setupClientSimGen`, `setupServer`, `sockaddr_in`, `sockaddr_in6`, `socketOpen`, `socket_accept`, `socket_blocking`, `socket_bus`, `socket_calc_udp_checksum`, `socket_channel`, `socket_check_udp_checksum`, `socket_close`, `socket_find_addresses`, `socket_open`, `socket_poll`, `socket_post`, `socket_publish`, `socket_recv`, `socket_recvfrom`, `socket_recvmmsg` … (+3 more)

**`math/bytelib.h`** — 1 used / 17 exported

*Used:* `uint16to`

**`support/elapsedtime.h`** — 2 used / 16 exported

*Used:* `ElapsedTime`, `split`

**`support/timelib.h`** — 2 used / 61 exported

*Used:* `secondsleep`, `timeval`


### `socketlib.h`

**`support/configCosmos.h`** — 1 used / 12 exported

*Used:* `close`


### `stringlib.cpp`

**`support/stringlib.h`** — 44 used / 44 exported

*Used:* `StringParser`, `byte_vector_to_string`, `clean_string`, `from_hex`, `from_hex_string`, `from_hex_vector`, `getFieldNumber`, `getFieldNumberAsDouble`, `getFieldNumberAsInteger`, `splitString`, `string_cmp`, `string_find`, `string_join`, `string_parse`, `string_replace`, `string_split`, `string_to_byte_vector`, `to_angle`, `to_astring`, `to_binary`, `to_bool`, `to_double`, `to_fixed`, `to_float`, `to_floatany`, `to_floatexp`, `to_floating`, `to_hex`, `to_hex_string`, `to_int16` … (+14 more)

**`math/mathlib.h`** — 1 used / 78 exported

*Used:* `size`

**`support/jsonobject.h`** — 3 used / 7 exported

*Used:* `JSONObject`, `addElement`, `to_json_object`


### `stringlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `timelib.cpp`

**`support/timelib.h`** — 50 used / 61 exported

*Used:* `DateTime`, `JD2MJD`, `MJD2JD`, `cal2mjd`, `calstruc`, `centisec`, `centisec2mjd`, `currentmjd`, `decisec`, `decisec2mjd`, `get_local_time`, `get_unix_time`, `gps2week`, `gregorianToModJulianDate`, `isleap`, `iso86012mjd`, `iso86012utc`, `jd2mjd`, `julcen`, `microsleep`, `mjd2cal`, `mjd2doy`, `mjd2iso8601`, `mjd2jd`, `mjd2tlef`, `mjd2year`, `mjd2ymd`, `mjdToGregorian`, `mjdToGregorianDDMMMYYYY`, `mjdToGregorianDDMmmYYYY` … (+20 more)

**`support/stringlib.h`** — 2 used / 44 exported

*Used:* `to_unixtime`, `to_unsigned`

**`math/mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)


### `timelib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/timeutils.h`** — 0 used / 7 exported

*Exported but none matched:* `TimeUtils`, `makeTimePoint`, `secondsSinceEpoch`, `secondsSinceMidnight`, `testSecondsSinceMidnight`, `timePointUtc`, `timeString`


### `timeutils.cpp`

**`support/timeutils.h`** — 7 used / 7 exported

*Used:* `TimeUtils`, `makeTimePoint`, `secondsSinceEpoch`, `secondsSinceMidnight`, `testSecondsSinceMidnight`, `timePointUtc`, `timeString`


### `timeutils.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`


### `transferclass.cpp`

**`support/transferclass.h`** — 39 used / 44 exported

*Used:* `INVALID_TXQ_IDX`, `Init`, `RESPONSE_REQUIRED`, `SendRetVal`, `Transfer`, `check_tx_id`, `close_file_pointers`, `enable_all`, `enable_single`, `get_packet_size`, `incoming_tx_add`, `incoming_tx_complete`, `incoming_tx_data`, `incoming_tx_del`, `incoming_tx_recount`, `incoming_tx_update`, `list_incoming`, `list_outgoing`, `node_id_to_txq_idx`, `outgoing_tx_add`, `outgoing_tx_data`, `outgoing_tx_del`, `outgoing_tx_load`, `outgoing_tx_recount`, `outgoing_tx_reqdata`, `print_file_packet`, `read_meta`, `receive_packet`, `reset_queue`, `save_progress` … (+9 more)


### `transferclass.h`

**`agent/agentclass.h`** — 1 used / 164 exported

*Used:* `send`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/datalib.h`** — 1 used / 62 exported

*Used:* `Init`

**`support/packetcomm.h`** — 1 used / 22 exported

*Used:* `PacketComm`

**`support/transferlib.h`** — 9 used / 57 exported

*Used:* `PACKET_CHUNK_SIZE_TYPE`, `PACKET_FILE_CRC_TYPE`, `PACKET_NODE_ID_TYPE`, `PACKET_TX_ID_TYPE`, `packet_struct_data`, `packet_struct_metadata`, `tx_entry`, `tx_progress`, `tx_queue`


### `transferlib.cpp`

**`support/transferlib.h`** — 49 used / 57 exported

*Used:* `FILE_TRANSFER_PROTOCOL_VERSION`, `PACKET_BYTE`, `PACKET_CHUNK_SIZE_TYPE`, `PACKET_FILE_CRC_TYPE`, `PACKET_FILE_SIZE_TYPE`, `PACKET_NODE_ID_TYPE`, `PACKET_QUEUE_FLAGS_LIMIT`, `PACKET_QUEUE_FLAGS_TYPE`, `PACKET_TX_ID_TYPE`, `add_chunk`, `add_chunks`, `clear_tx_entry`, `deserialize_cancel`, `deserialize_complete`, `deserialize_data`, `deserialize_metadata`, `deserialize_metafile`, `deserialize_queue`, `deserialize_reqcomplete`, `deserialize_reqdata`, `deserialize_reqmeta`, `file_packet_header`, `file_progress`, `filestruc_smaller_by_size`, `find_chunks_missing`, `flush_tx_entry`, `get_file_size`, `lower_chunk`, `merge_chunks_overlap`, `packet_struct_cancel` … (+19 more)

**`math/bytelib.h`** — 2 used / 17 exported

*Used:* `uint32from`, `uint32to`


### `transferlib.h`

**`support/configCosmos.h`** — 0 used / 12 exported

*Exported but none matched:* `CLOSE_SOCKET`, `COSMOS_MKDIR`, `COSMOS_SLEEP`, `COSMOS_USLEEP`, `Sleep`, `_mkdir`, `close`, `closesocket`, `defined`, `mkdir`, `sleep`, `usleep`

**`support/datalib.h`** — 0 used / 62 exported

*Exported but none matched:* `CloseIfStrideTime`, `DataLog`, `GITTEST`, `Init`, `SetFastmode`, `SetStartdate`, `SetStride`, `Write`, `data_archive_path`, `data_base_path`, `data_ctime`, `data_execute`, `data_exists`, `data_getcwd`, `data_isblkdev`, `data_ischardev`, `data_isdir`, `data_isfile`, `data_issymlink`, `data_list_archive` … (+42 more)

**`support/packetcomm.h`** — 1 used / 22 exported

*Used:* `PacketComm`

**`support/elapsedtime.h`** — 1 used / 16 exported

*Used:* `ElapsedTime`


## support/convert_test_gui

### `eci2kep_test.cpp`

**`eci2kep_test.h`** — 4 used / 5 exported

*Used:* `eci2kep_test`, `on_SelectMax_clicked`, `on_SelectMin_clicked`, `on_comboBox_currentIndexChanged`


### `eci2kep_test.h`

**`testcontainer.h`** — 1 used / 16 exported

*Used:* `TestContainer`

**`ui_eci2kep_test.h`** — header not indexed (third-party or missing)


### `filepathdialog.cpp`

**`filepathdialog.h`** — 2 used / 2 exported

*Used:* `FilepathDialog`, `getFilepath`

**`ui_filepathdialog.h`** — header not indexed (third-party or missing)


### `main.cpp`

**`mainwindow.h`** — 1 used / 8 exported

*Used:* `MainWindow`


### `mainwindow.cpp`

**`mainwindow.h`** — 8 used / 8 exported

*Used:* `MainWindow`, `on_ChangeFolder_clicked`, `on_CollapseAll_clicked`, `on_DeselectAll_clicked`, `on_RunAll_clicked`, `on_RunSelected_clicked`, `on_SelectAll_clicked`, `recountTests`

**`ui_mainwindow.h`** — header not indexed (third-party or missing)

**`filepathdialog.h`** — 2 used / 2 exported

*Used:* `FilepathDialog`, `getFilepath`

**`testcontainer.h`** — 5 used / 16 exported

*Used:* `TestContainer`, `runTest`, `selectionChanged`, `setSelected`, `toggleHidden`


### `rearth_test.cpp`

**`rearth_test.h`** — 4 used / 4 exported

*Used:* `on_SelectMax_clicked`, `on_SelectMin_clicked`, `on_TestList_currentItemChanged`, `rearth_test`


### `rearth_test.h`

**`testcontainer.h`** — 1 used / 16 exported

*Used:* `TestContainer`

**`ui_rearth_test.h`** — header not indexed (third-party or missing)


### `testcontainer.cpp`

**`testcontainer.h`** — 16 used / 16 exported

*Used:* `TestContainer`, `angleDelta`, `fadeBanner`, `keplerDelta`, `listWriteCartpos`, `listWriteKepstruc`, `listWriteRVec`, `on_Expand_clicked`, `on_checkBox_stateChanged`, `openFileLine`, `runTest`, `selectionChanged`, `setError`, `setSelected`, `skipLines`, `toggleHidden`

**`ui_testcontainer.h`** — header not indexed (third-party or missing)

**`mainwindow.h`** — 1 used / 8 exported

*Used:* `MainWindow`

**`rearth_test.h`** — 1 used / 4 exported

*Used:* `rearth_test`

**`eci2kep_test.h`** — 1 used / 5 exported

*Used:* `eci2kep_test`


### `testcontainer.h`

**`convertlib.h`** — 0 used / 157 exported

*Exported but none matched:* `Position`, `Rearth`, `Vector`, `atan3`, `att_clear`, `att_extra`, `att_geoc`, `att_geoc2icrf`, `att_icrf`, `att_icrf2geoc`, `att_icrf2lvlh`, `att_icrf2selc`, `att_lvlh`, `att_lvlh2icrf`, `att_lvlh2planec`, `att_planec2lvlh`, `att_planec2topo`, `att_selc`, `att_selc2icrf`, `att_set_geoc` … (+137 more)

**`mathlib.h`** — 0 used / 78 exported

*Exported but none matched:* `LsFit`, `accel`, `accelgvector`, `accelquaternion`, `accelrvector`, `actan`, `area_rv`, `distance_rv`, `dodct`, `drotate`, `estimatorhandle`, `estimatorstruc`, `eval`, `evalgvector`, `evalquaternion`, `evalrvector`, `evaluate_poly`, `evaluate_poly_accel`, `evaluate_poly_jerk`, `evaluate_poly_slope` … (+58 more)

**`mainwindow.h`** — 1 used / 8 exported

*Used:* `MainWindow`


## support/filesenderimpl

### `FileSender.cpp`

**`FileSender.h`** — 3 used / 3 exported

*Used:* `FileSender`, `init`, `send`


### `FileSender.h`

**`support/transferclass.h`** — 3 used / 44 exported

*Used:* `SendRetVal`, `Sender`, `send`


### `UdpSender.cpp`

**`UdpSender.h`** — 3 used / 3 exported

*Used:* `UdpSender`, `init`, `send`


### `UdpSender.h`

**`support/socketlib.h`** — 2 used / 37 exported

*Used:* `send`, `socket_channel`

**`support/transferclass.h`** — 3 used / 44 exported

*Used:* `SendRetVal`, `Sender`, `send`


