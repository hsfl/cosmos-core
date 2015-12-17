# Add a new device {#tutorial-namespace-add-new-device}

As an example we are going to add a new generic device to measure the temperature named "temperatureStation". Go to jsondef.h approx in line 960 and create the structure that  contains the information you want to use.

```
struct temperatureStationStruc
{
	//! Generic info must be here for every device
	genstruc gen;
	//! the following is any data specific to this device
	float temperature; // your temperature data will be stored here
} ;
```

add your temperatureStationStruc structure to the devicestruc union (apporox in line 1400)

```
typedef struct
{
	union
	{
		allstruc all;
		...
		thststruc thst;
		tsenstruc tsen;
		temperatureStationStruc temperatureStation; // << --- add here
	};
} devicestruc;
```

add your temperatureStationStruc structure to the devspecstruc structure (apporox in line 1500)

```
typedef struct
{
	uint16_t ant_cnt;
	...
	uint16_t thst_cnt;
	uint16_t tsen_cnt; 
	uint16_t temperatureStation_cnt; // << --- add here
	vector<allstruc *>all;
	...
	vector<thststruc *>thst;
	vector<tsenstruc *>tsen;
	vector<temperatureStationStruc *>temperatureStation;  // << --- add here
} devspecstruc;
```

now go to jsonlib.cpp , add your temperatureStation to the end of device\_type\_string

```
vector <string> device_type_string
{
	"pload",
	...
	"cam",
	"temperatureStation" // <-- add here
};
```

in jsondef.h you also must add the device type to the end of device\_type\ enum (approx in line 400)

```
enum
	{
	//! Payload
	DEVICE_TYPE_PLOAD=0,
	...
	//! Camera
	DEVICE_TYPE_CAM=26,
	//! your tempStation here
	DEVICE_TYPE_TEMPSTATION = 27, // <- add here 
	//! List count
	DEVICE_TYPE_COUNT,
	//! Not a Component
	DEVICE_TYPE_NONE=65535
	};
```

now we are going to modify some functions in the code. The first one is json\_detroy in jsonlib.cpp

```
void json_destroy(cosmosstruc *cdata)
{
	for (uint16_t i=0; i<2; ++i)
	{
		cdata[i].devspec.ant.resize(0);
		...
		cdata[i].devspec.tsen.resize(0);
		cdata[i].devspec.temperatureStation.resize(0); // <- add here
		cdata[i].device.resize(0);

	}

	delete [] cdata;
	cdata = NULL;
}
```

(side note: for a really complex type further definitions must be added to the namespace, but most common types are already supported, so this is an advanced feature)

go to json\_devices\_specific and inside the for loop that goes over each type add some of the following

```
const char *json_devices_specific(string &jstring, cosmosstruc *cdata)
{
...

	for (uint16_t j=0; j<*cnt; ++j)
	{
	...
  
		// Dump Temperature Station
	 	if (!strcmp(device_type_string[i].c_str(),"tempStation"))
	 	{
		 	json_out_1d(jstring,(char *)"device_tempStation_temperature",j,cdata);
	 		json_out_character(jstring, '\n');
	 	}

	} 
}
```

go to json\_clone

```
int32_t json_clone(cosmosstruc *cdata)
{
...
	case DEVICE_TYPE_TEMPSTATION:
		cdata[1].devspec.tempStation[cdata[1].device[i].all.gen.didx] =
			&cdata[1].device[i].tempStation;
		break;
...
}
```

add name for the device count in json\_addbaseentry

```
uint16_t json_addbaseentry(cosmosstruc *cdata)
{

	...
	json_addentry("device_tempStation_cnt", 
		UINT16_MAX, 
		UINT16_MAX,
		offsetof(devspecstruc,tempStation_cnt), 
		COSMOS_SIZEOF(uint16_t), 
		(uint16_t)JSON_TYPE_UINT16,
		JSON_GROUP_DEVSPEC,
		cdata);

}
```

to json\_adddeviceentry add 

```
uint16_t json_adddeviceentry(uint16_t i, cosmosstruc *cdata)
{

	...
	case DEVICE_TYPE_TEMPSTATION:
	
		json_addentry("device_tempStation_utc",
			didx, 
			UINT16_MAX, 
			(ptrdiff_t)offsetof(genstruc,utc)+i*sizeof(devicestruc),
			COSMOS_SIZEOF(double), 
			(uint16_t)JSON_TYPE_DOUBLE,
			JSON_GROUP_DEVICE,
			cdata);

		json_addentry("device_tempStation_temperature",
			didx, 
			UINT16_MAX,
			(ptrdiff_t)offsetof(temperatureStationStruc,temperature) + 
				i*sizeof(devicestruc),
			COSMOS_SIZEOF(double), 
			(uint16_t)JSON_TYPE_DOUBLE,
			JSON_GROUP_DEVICE,
			cdata);
		
		cdata[0].devspec.tempStation.push_back(
			(temperatureStationStruct *)&cdata[0].device[i].tempStation);
		cdata[0].devspec.tempStation_cnt = 
			(uint16_t)cdata[0].devspec.tempStation.size();
	break;

}
```