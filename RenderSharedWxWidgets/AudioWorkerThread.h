#pragma once

//#include "MainFrame.h"

#include "wx/thread.h"

#include "WASAPIRenderer.h"


#include <iostream>
#include <fstream>

#include <list>



//////////////////////////////////////////////////////////////////////////////////




//class CThreadParamObject : public CObject
class CThreadParamObject
{


public:
	CThreadParamObject() : frequency(0), durationInSec(0) { }
	CThreadParamObject(int f, int d) : frequency(f), durationInSec(d) { }

	int getFrequency() { return frequency; }
	int getDurationInSec() { return durationInSec; }

private:
	int masterVolue = 100;
	int frequency;
	int durationInSec;

};



////////////////////////////////////////////////////////////////////////////////////


class MainFrame;


class AudioWorkerThread : public wxThread
{
public:
    AudioWorkerThread(MainFrame* frame);
                   

    // thread execution starts here
    virtual void* Entry() wxOVERRIDE;

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit() wxOVERRIDE;

    bool postCommand(int commandID);
	bool postCommand(int commandID, std::list<int> params);



private:

	void PlayToneThreadProc(int TargetDurationInSec, int TargetFrequency, int signalType);


    void postEvent(int eventID);
    void log(wxString message);
    void log(wxString message, int value);


    MainFrame* m_frame;
	unsigned m_audioThreadTickCount = 0;    // This field is to keep track of the time of the audio thread...
											// But perhaps using the Win32 System time may be more appropriate

	bool wasapInit = false;

    std::list<int> commands;
    std::ofstream logFile;
    
    int nextCommand;

	////////////////////////////////////////////////////////////////////////////  
	// Those functions and variables may be transfered to a WASAPI sound class
	////////////////////////////////////////////////////////////////////////////  

	bool initWASAPI();
	bool terminateWASAPI();
	bool PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole);
	LPWSTR GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex);
	int generateSignal(int signalFreq, int channelCount,
		CWASAPIRenderer::RenderSampleType SampleType,
		int samplesPerSecond,
		int frameSize,
		int bufferSizeInBytes, int totNumberOfBuffers, 
		int signalType,bool addNoiseIndicator);
	bool playTone(int freq, int durationInSec);
	DWORD WINAPI PlayToneThreadProc(LPVOID lpParam);

	IMMDevice* device = NULL;
	bool isDefaultDevice;
	ERole role;
	UINT32 renderBufferSizeInBytes;
	size_t renderDataLength;
	size_t renderBufferCount;


	CWASAPIRenderer* renderer = NULL;
	RenderBuffer* renderQueue = NULL;
	HANDLE hPlayThread = NULL;
	DWORD dwTPlayhreadID;

	CThreadParamObject* pParamObject = NULL;


	int  MasterVolume = 100;
	int  TargetFrequency = 220;
	int  TargetLatency = 50;
	int  TargetDurationInSec = 2;
	bool ShowHelp;
	bool UseConsoleDevice;
	bool UseCommunicationsDevice;
	bool UseMultimediaDevice;
	bool DisableMMCSS;
	int  signalType;
	int  result = 0;

	wchar_t* OutputEndpoint;


	int m_signalType;
	int m_noiseType;

	bool addNoiseIndicator = false;


	////////////////////////////////////////////////////////////////////  
    
};

