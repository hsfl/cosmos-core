//////////////////////////////////////////////////////////////////////
//
//	acq_a35.h : header file
//
//////////////////////////////////////////////////////////////////////

#include "agentlib.h"
#include "gige_lib.h"

//! \ingroup devices
//! \defgroup acq_a35 A35 camera support

//! \ingroup acq_a35
//! \defgroup acq_a35_functions A35 library function declarations
//! @{
bool InitCamera(uint16_t width, uint16_t height, gige_handle *handle);
bool GetCameraFrame(gige_handle* handle, uint16_t *ImageFrameBuffer, uint32_t ImageSize);
void WriteRegister(gige_handle* handle,  int16_t command, int16_t param1, int16_t param2, int16_t param3);
void StartCamera(gige_handle *handle);
int ShutdownCamera(gige_handle *handle);

bool ResetCamera(gige_handle *handle);
bool AutoFFC(gige_handle *handle);
bool ManualFFC(gige_handle *handle);
bool DoFFC(gige_handle *handle);
bool GetTemperature(gige_handle *handle);
bool SetFramesToSkip(gige_handle* handle, int FramesToSkip);
bool SetPacketDelay(gige_handle* handle, int PacketDelay);
//! @}
