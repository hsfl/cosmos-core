//////////////////////////////////////////////////////////////////////
//
//	IreGigEAcq.h : NI-IMAQ Camera header file
//
//////////////////////////////////////////////////////////////////////

#pragma once

class Ca35acq
{
public:
    Ca35acq();
    virtual ~Ca35acq();

	int InitCamera(unsigned int *AcqWinWidth, unsigned int *AcqWinHeight);
	unsigned long GetCameraFrame(unsigned short *m_nPhotonFrameBuffer);
	void StartCamera();
	int StopCamera();

    bool ResetCamera();
    bool AutoFFC();
    bool ManualFFC();
    bool DoFFC();

protected:

};
