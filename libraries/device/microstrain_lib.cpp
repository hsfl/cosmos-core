/*! \file microstrain_lib.c
	\brief Support routines for MicroStrain IMU
*/

#include "microstrain_lib.h"

#include "configCosmos.h"
#if defined (COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

//! \ingroup microstrain
//! \defgroup microstrain_statics MicroStrain IMU statics
//! @{

//! IMU state indicator
static microstrain_state *imu_state[IMUMAXCOUNT] = {NULL,NULL,NULL,NULL};

//! @}

//! \addtogroup microstrain_functions
//! @{

//! Connect to IMU
/*! Establishes connection to microstrain IMU on specific serial port.
	\param dev device name of serial port
	\return handle to use for that IMU, otherwise -1
	\see cssl_start
	\see cssl_open
*/
int microstrain_connect(const char *dev)
{
int i, iretn;
double taccel, tgyrox, tgyroy, tgyroz;

cssl_start();
for (i=0; i<IMUMAXCOUNT; i++)
	{
	if (imu_state[i] != NULL)
		{
		if (!strcmp(dev,imu_state[i]->dev))
			{
			return (MICROSTRAIN_ERROR_OPEN);
			}
		}
	else
		{
		break;
		}
	}

if (i == IMUMAXCOUNT)
	return (MICROSTRAIN_ERROR_TOOMANY);

if ((imu_state[i]=(microstrain_state *)calloc(1,sizeof(microstrain_state))) == NULL)
	return (MICROSTRAIN_ERROR_FAILED);

imu_state[i]->handle = cssl_open(dev,MICROSTRAIN_BAUD,MICROSTRAIN_BITS,MICROSTRAIN_PARITY,MICROSTRAIN_STOPBITS);

if (imu_state[i]->handle != NULL)
	{
	imu_state[i]->dev = (char *)malloc(strlen(dev));
	strcpy(imu_state[i]->dev,dev);
	iretn = microstrain_temperature(i,&taccel,&tgyrox,&tgyroy,&tgyroz);
	if (iretn <0)
		{
		microstrain_disconnect(i);
		return (MICROSTRAIN_ERROR_FUNCTION);
		}
	return (i);
	}

free(imu_state[i]);
imu_state[i] = NULL;
return (MICROSTRAIN_ERROR_FAILED);
}

//! Disconnect IMU
/*! Close specified IMUs serial port and recover resources.
	\param handle integer handle returned from previous microstrain_connect
	\return 0, otherwise error
	\see cssl_close
	\see microstrain_connect
*/
int microstrain_disconnect(int handle)
{
if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

cssl_close(imu_state[handle]->handle);
free(imu_state[handle]->dev);
free(imu_state[handle]);
imu_state[handle] = NULL;
return 0;
}

//! Send command
/*! Send message currently queued in the state handle and accept a reply.
	\param handle Handle of requested IMU
	\param rsize Expected length of returned message
	\param rtime Time to wait for reply in milliseconds.
	\return bytes of data returned, otherwise negative error number
*/
int microstrain_send_message(int handle, uint16_t rsize, uint16_t rtime)
{
int i, j;
uint16_t checksum;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

cssl_settimeout(imu_state[handle]->handle,0,1.+rtime/10.);
cssl_putdata(imu_state[handle]->handle,&(imu_state[handle]->data[0]),imu_state[handle]->size);

i = 0;
while ((j=cssl_getdata(imu_state[handle]->handle,(uint8_t *)&(imu_state[handle]->data[i]),rsize-i)) > 0)
	{
	i += j;
	}

if (i != rsize)
	return (-i);

imu_state[handle]->checksum = uint16from((uint8_t *)(&imu_state[handle]->data[rsize-2]),(uint8_t)ORDER_BIGENDIAN);

checksum = microstrain_calc_checksum(imu_state[handle]->data,rsize-2);

if (imu_state[handle]->checksum != checksum)
	return (-i);

imu_state[handle]->timer = uint32from(&imu_state[handle]->data[rsize-6],ORDER_BIGENDIAN);
imu_state[handle]->data[rsize] = 0;
imu_state[handle]->size = rsize;
return (rsize);
}

//! Calculate MicroStrain Checksum
/*! Calculate unsigned 16-bit Checksum for the indicated buffer and number of bytes. Add all
 * bytes, rolling to 0 at 65535.
 	\param buf bytes to calculate on
	\param size number of bytes
	\return calculated Checksum
*/

uint16_t microstrain_calc_checksum(uint8_t *buf, uint16_t size)
{
uint16_t checksum = 0x0;
int i;

for (i=0; i<size; i++)
	{
	checksum += buf[i];
	}
return (checksum);
}

//! Set MicroStrain size
/*! Set the  size value for the requested IMU.
	\param handle The handle for the requested device
	\param size The address to set
	\return 0 if successful, otherwise negative error value
*/

int microstrain_set_size(int handle, uint16_t size)
{
if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->size = size;
imu_state[handle]->data[size] = 0;
return 0;
}

//! Get MicroStrain Timer
/*! Get the  Timer value for the requested IMU.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int32_t microstrain_get_timer(int handle)
{
if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

return (imu_state[handle]->timer);
}

//! Get MicroStrain Checksum 
/*! Get the  Checksum value for the requested IMU.
	\param handle The handle for the requested device
	\return Checksum if successful, otherwise zero.
*/

uint16_t microstrain_get_checksum(int handle)
{
if (imu_state[handle] == NULL)
	return 0;

return (imu_state[handle]->checksum);
}

//! Get MicroStrain size
/*! Get the data size for the requested IMU.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int microstrain_get_size(int handle)
{
if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

return (imu_state[handle]->size);
}

//! GET MicroStrain data field
/*! Get the pointer to the data field for the requested IMU.
	\param handle The handle for the requested device
	\return pointer to the beginning of the data are, otherwise NULL
*/
uint8_t *microstrain_get_data(int handle)
{
if (imu_state[handle] == NULL)
	return ((uint8_t *)NULL);
return (imu_state[handle]->data);
}

//! MicroStrain Raw Linear Accelaration and Angular Rate  command
/*! Retrieve the raw sensor voltages for linear accelaration and angular rate in A/D converter codes.
	\param handle Handle of requested IMU
	\param accel Raw linear acceleration values for x, y, and z. 0. to 5. volts
	\param rate Raw angular rate values for x, y, and z. 0. to 5. volts
	\return 0 if successful
*/

int microstrain_raw_accel_rate(int handle, rvector *accel, rvector *rate)
{
int32_t iretn;
float scale;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_RAW_ACC_RAT;
microstrain_set_size(handle,MICROSTRAIN_RAW_ACC_RAT_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_RAW_ACC_RAT_BIN,0)) != MICROSTRAIN_RAW_ACC_RAT_BIN)
	return (MICROSTRAIN_ERROR_SEND);

scale = 5./65535.;
accel->col[0] = scale * floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
accel->col[1] = scale * floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
accel->col[2] = scale * floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
rate->col[0] = scale * floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
rate->col[1] = scale * floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
rate->col[2] = scale * floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Linear Acceleration and Angular Rate  command
/*! Retrieve the linear acceleration and angular rate.
	\param handle Handle of requested IMU
	\param accel linear acceleration values for x, y, and z, in meters/second/second.
	\param rate angular rate values for x, y, and z, in radians/second.
	\return 0 if successful
*/

int microstrain_accel_rate(int handle, rvector *accel, rvector *rate)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_ACC_RAT;
microstrain_set_size(handle,MICROSTRAIN_ACC_RAT_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_ACC_RAT_BIN,0)) != MICROSTRAIN_ACC_RAT_BIN)
	return (MICROSTRAIN_ERROR_SEND);

accel->col[0] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
accel->col[1] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
accel->col[2] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
rate->col[0] = floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
rate->col[1] = floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
rate->col[2] = floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Delta angle/Velocity  command
/*! Retrieve the change of angle and velocity over the most recent measurement period.
	\param handle Handle of requested IMU
	\param deltaa Change of the x,y,z components of angular rotation in radians.
	\param deltav Change of the x,y,z components of linear velocity in meters/second.
	\return 0 if successful
*/

int microstrain_delta_angle_velocity(int handle, rvector *deltaa, rvector *deltav)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_DELTA_ANG_VEL;
microstrain_set_size(handle,MICROSTRAIN_DELTA_ANG_VEL_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_DELTA_ANG_VEL_BIN,0)) != MICROSTRAIN_DELTA_ANG_VEL_BIN)
	return (MICROSTRAIN_ERROR_SEND);

deltaa->col[0] = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
deltaa->col[1] = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
deltaa->col[2] = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
deltav->col[0] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
deltav->col[1] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
deltav->col[2] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Orientation command
/*! Retrieve the current orientation matrix.
	\param handle Handle of requested IMU
	\param matrix Direction cosine matrix for current orientation
	\return 0 if successful
*/

int microstrain_orientation(int handle, rmatrix *matrix)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_ORI;
microstrain_set_size(handle,MICROSTRAIN_ORI_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_ORI_BIN,0)) != MICROSTRAIN_ORI_BIN)
	return (MICROSTRAIN_ERROR_SEND);

matrix->row[0].col[0] = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
matrix->row[0].col[1] = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
matrix->row[0].col[2] = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
matrix->row[1].col[0] = floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
matrix->row[1].col[1] = floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
matrix->row[1].col[2] = floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);
matrix->row[2].col[0] = floatfrom(&imu_state[handle]->data[25],ORDER_BIGENDIAN);
matrix->row[2].col[1] = floatfrom(&imu_state[handle]->data[29],ORDER_BIGENDIAN);
matrix->row[2].col[2] = floatfrom(&imu_state[handle]->data[33],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Orientation Update command
/*! Retrieve the orientation update matrix for the latest calculation cycle.
	\param handle Handle of requested IMU
	\param matrix Direction cosine matrix for orientation update
	\return 0 if successful
*/

int microstrain_orientation_update(int handle, rmatrix *matrix)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_ORI_UPDATE;
microstrain_set_size(handle,MICROSTRAIN_ORI_UPDATE_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_ORI_UPDATE_BIN,0)) != MICROSTRAIN_ORI_UPDATE_BIN)
	return (MICROSTRAIN_ERROR_SEND);

matrix->row[0].col[0] = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
matrix->row[0].col[1] = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
matrix->row[0].col[2] = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
matrix->row[1].col[0] = floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
matrix->row[1].col[1] = floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
matrix->row[1].col[2] = floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);
matrix->row[2].col[0] = floatfrom(&imu_state[handle]->data[25],ORDER_BIGENDIAN);
matrix->row[2].col[1] = floatfrom(&imu_state[handle]->data[29],ORDER_BIGENDIAN);
matrix->row[2].col[2] = floatfrom(&imu_state[handle]->data[33],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Magnetic Field command
/*! Retrieve the instantaneous magnetometer reading in the IMU body frame.
	\param handle Handle of requested IMU
	\param field x,y,z elements of magnetic field in Gauss
	\return 0 if successful
*/

int microstrain_magfield(int handle, rvector *field)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_MAG;
microstrain_set_size(handle,MICROSTRAIN_MAG_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_MAG_BIN,0)) != MICROSTRAIN_MAG_BIN)
	return (MICROSTRAIN_ERROR_SEND);


field->col[0] = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
field->col[1] = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
field->col[2] = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Acceleration, Angular Rate, and Magnetometer command
/*! Retrieve the current linear acceleration, angular rate, and
 * magnetic field vectors.
	\param handle Handle of requested IMU
	\param accel Linear acceleration x, y, and z in meters/second/second
	\param rate Angular rate of rotation x, y, and z in radians/second
	\param field Magnetic field in body frame of IMU, in Gauss
	\return 0 if successful
*/

int microstrain_accel_rate_magfield(int handle, rvector *accel, rvector *rate, rvector *field)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_ACC_RAT_MAG;
microstrain_set_size(handle,MICROSTRAIN_ACC_RAT_MAG_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_ACC_RAT_MAG_BIN,0)) != MICROSTRAIN_ACC_RAT_MAG_BIN)
	return (MICROSTRAIN_ERROR_SEND);

accel->col[0] = MICROSTRAIN_G * floatfrom((uint8_t *)&imu_state[handle]->data[1],ORDER_BIGENDIAN);
accel->col[1] = MICROSTRAIN_G * floatfrom((uint8_t *)&imu_state[handle]->data[5],ORDER_BIGENDIAN);
accel->col[2] = MICROSTRAIN_G * floatfrom((uint8_t *)&imu_state[handle]->data[9],ORDER_BIGENDIAN);

rate->col[0] = floatfrom((uint8_t *)&imu_state[handle]->data[13],ORDER_BIGENDIAN);
rate->col[1] = floatfrom((uint8_t *)&imu_state[handle]->data[17],ORDER_BIGENDIAN);
rate->col[2] = floatfrom((uint8_t *)&imu_state[handle]->data[21],ORDER_BIGENDIAN);

field->col[0] = floatfrom((uint8_t *)&imu_state[handle]->data[25],ORDER_BIGENDIAN);
field->col[1] = floatfrom((uint8_t *)&imu_state[handle]->data[29],ORDER_BIGENDIAN);
field->col[2] = floatfrom((uint8_t *)&imu_state[handle]->data[33],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Stablized Acceleration, Angular Rate, and Magnetometer command
/*! Retrieve the current gyro stabilized linear acceleration, angular
 * rate, and magnetic field vectors.
	\param handle Handle of requested IMU
	\param accel Linear acceleration x, y, and z in meters/second/second
	\param rate Angular rate of rotation x, y, and z in radians/second
	\param field Magnetic field in body frame of IMU, in Gauss
	\return 0 if successful
*/

int microstrain_stab_accel_rate_magfield(int handle, rvector *accel, rvector *rate, rvector *field)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_STAB_ACC_RAT_MAG;
microstrain_set_size(handle,MICROSTRAIN_STAB_ACC_RAT_MAG_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_STAB_ACC_RAT_MAG_BIN,0)) != MICROSTRAIN_STAB_ACC_RAT_MAG_BIN)
	return (MICROSTRAIN_ERROR_SEND);

accel->col[0] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
accel->col[1] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
accel->col[2] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
rate->col[0] = floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
rate->col[1] = floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
rate->col[2] = floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);

field->col[0] = floatfrom(&imu_state[handle]->data[25],ORDER_BIGENDIAN);
field->col[1] = floatfrom(&imu_state[handle]->data[29],ORDER_BIGENDIAN);
field->col[2] = floatfrom(&imu_state[handle]->data[33],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Acceleration, Angular Rate, Magnetometer and Orientation command
/*! Retrieve the current linear acceleration, angular rate, magnetic field and orientation matrix.
	\param handle Handle of requested IMU
	\param accel Linear acceleration x, y, and z in meters/second/second
	\param rate Angular rate of rotation x, y, and z in radians/second
	\param field Magnetic field in body frame of IMU, in Gauss
	\param matrix Direction cosine matrix for current orientation
	\return 0 if successful
*/

int microstrain_accel_rate_magfield_orientation(int handle, rvector *accel, rvector *rate, rvector *field, rmatrix *matrix)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_ACC_RAT_MAG_ORI;
microstrain_set_size(handle,MICROSTRAIN_ACC_RAT_MAG_ORI_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_ACC_RAT_MAG_ORI_BIN,0)) != MICROSTRAIN_ACC_RAT_MAG_ORI_BIN)
	return (MICROSTRAIN_ERROR_SEND);

accel->col[0] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
accel->col[1] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
accel->col[2] = MICROSTRAIN_G * floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
rate->col[0] = floatfrom(&imu_state[handle]->data[13],ORDER_BIGENDIAN);
rate->col[1] = floatfrom(&imu_state[handle]->data[17],ORDER_BIGENDIAN);
rate->col[2] = floatfrom(&imu_state[handle]->data[21],ORDER_BIGENDIAN);

field->col[0] = floatfrom(&imu_state[handle]->data[25],ORDER_BIGENDIAN);
field->col[1] = floatfrom(&imu_state[handle]->data[29],ORDER_BIGENDIAN);
field->col[2] = floatfrom(&imu_state[handle]->data[33],ORDER_BIGENDIAN);

matrix->row[0].col[0] = floatfrom(&imu_state[handle]->data[37],ORDER_BIGENDIAN);
matrix->row[0].col[1] = floatfrom(&imu_state[handle]->data[41],ORDER_BIGENDIAN);
matrix->row[0].col[2] = floatfrom(&imu_state[handle]->data[45],ORDER_BIGENDIAN);
matrix->row[1].col[0] = floatfrom(&imu_state[handle]->data[49],ORDER_BIGENDIAN);
matrix->row[1].col[1] = floatfrom(&imu_state[handle]->data[53],ORDER_BIGENDIAN);
matrix->row[1].col[2] = floatfrom(&imu_state[handle]->data[57],ORDER_BIGENDIAN);
matrix->row[2].col[0] = floatfrom(&imu_state[handle]->data[61],ORDER_BIGENDIAN);
matrix->row[2].col[1] = floatfrom(&imu_state[handle]->data[65],ORDER_BIGENDIAN);
matrix->row[2].col[2] = floatfrom(&imu_state[handle]->data[69],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Capture Gyro Bias command
/*! Calibrate the Gyros to determine orientation for the specified amount of time. The resulting
 * value will then be written to the Gyro biases.
 	\param handle Handle of requested IMU
	\param msec Number of milliseconds to perform calibration (recommend 10000 to 30000).
	\param bias x,y,z results of calibration
	\return 0 if successful
*/

int microstrain_capture_gyro_bias(int handle, uint16_t msec, rvector *bias)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_INIT_GYRO_BIAS;
imu_state[handle]->data[1] = 0xc1;
imu_state[handle]->data[2] = 0x29;
uint16to(msec,&imu_state[handle]->data[2],ORDER_BIGENDIAN);

microstrain_set_size(handle,MICROSTRAIN_INIT_GYRO_BIAS_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_INIT_GYRO_BIAS_BIN,msec+5000)) != MICROSTRAIN_INIT_GYRO_BIAS_BIN)
	return (MICROSTRAIN_ERROR_SEND);

bias->col[0] = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
bias->col[1] = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
bias->col[2] = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);
return 0;
}

//! MicroStrain Euler Angle command
/*! Retrieve Euler angle respresentation of current orientation.
	\param handle Handle of requested IMU
	\param euler Euler angles: roll, pitch, yaw
	\return 0 if successful
*/

int microstrain_euler_angles(int handle, avector *euler)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_EULER_ANG;

microstrain_set_size(handle,MICROSTRAIN_EULER_ANG_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_EULER_ANG_BIN,0)) != MICROSTRAIN_EULER_ANG_BIN)
	return (MICROSTRAIN_ERROR_SEND);

euler->b = floatfrom(&imu_state[handle]->data[1],ORDER_BIGENDIAN);
euler->e = floatfrom(&imu_state[handle]->data[5],ORDER_BIGENDIAN);
euler->h = floatfrom(&imu_state[handle]->data[9],ORDER_BIGENDIAN);

return 0;
}

//! MicroStrain Temperature  command
/*! Retrieve the temperatures of the acclerometer and 3 gyros, in degrees Celsius.
	\param handle Handle of requested IMU
	\param tempa Temperature of accelerometer
	\param tempgx Temperature of X Gyro
	\param tempgy Temperature of Y Gyro
	\param tempgz Temperature of Z Gyro
	\return 0 if successful
*/

int microstrain_temperature(int handle, double *tempa, double *tempgx, double *tempgy, double *tempgz)
{
float scale;
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_TEMP;
microstrain_set_size(handle,MICROSTRAIN_TEMP_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_TEMP_BIN,0)) != MICROSTRAIN_TEMP_BIN)
	return (MICROSTRAIN_ERROR_SEND);

scale = 3.3/4096.;
*tempa = 100. * (uint16from(&imu_state[handle]->data[1],ORDER_BIGENDIAN) * scale - .5);
*tempgx = (uint16from(&imu_state[handle]->data[3],ORDER_BIGENDIAN) * scale - 2.5)/.009 + 25.;
*tempgy = (uint16from(&imu_state[handle]->data[5],ORDER_BIGENDIAN) * scale - 2.5)/.009 + 25.;
*tempgz = (uint16from(&imu_state[handle]->data[7],ORDER_BIGENDIAN) * scale - 2.5)/.009 + 25.;

return 0;
}

//! MicroStrain Device ID command
/*! Retrieve one of 4 possible device ID strings: Model Number, Serial Number, Model Name, or
 * Device Options.
 	\param handle Handle number of IMU
	\param selector One of MICROSTRAIN_MODEL_NUMBER, MICROSTRAIN_SERIAL_NUMBER, MICROSTRAIN_MODEL_NAME, MICROSTRAIN_DEVICE_OPTIONS
	\param buf Buffer for device identifier string.
	\return 0 if successful, otherwise negative error number
*/
int microstrain_device_identifier(int handle, uint8_t selector, char *buf)
{
int32_t iretn;

if (imu_state[handle] == NULL)
	return (MICROSTRAIN_ERROR_CLOSED);

imu_state[handle]->data[0] = MICROSTRAIN_DEVICEID;
imu_state[handle]->data[1] = selector;
microstrain_set_size(handle,MICROSTRAIN_DEVICEID_BOUT);

if ((iretn=microstrain_send_message(handle,MICROSTRAIN_DEVICEID_BIN,0)) != MICROSTRAIN_DEVICEID_BIN)
	return (MICROSTRAIN_ERROR_SEND);

strncpy(buf,(char *)&(imu_state[handle]->data[2]),16);
buf[iretn-4] = 0;
return 0;
}

//! @}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)

