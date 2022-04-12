
// CMainDlg.h : header file
//

#pragma once

////////////////////////////////////////////////////////////////////////////  
// Those includes may be transfered to a WASAPI sound class
////////////////////////////////////////////////////////////////////////////  

#include <iostream>
#include "WASAPIRenderer.h"
#include <functiondiscoverykeys.h>
#include <new>          // std::nothrow

#include <strsafe.h>

#include "CSpectrumGraphCtrl.h"

#include "WavLoader.h"



#define MINIMAL_FREQUENCY                 220
#define MAXMIMAL_FREQUENCY                880
#define MINIMAL_VOLUME                      0
#define MAXMIMAL_VOLUME                    10

#define HARMONIC_MIN_LEVEL				    0
#define HARMONIC_MAX_LEVEL				  100


//#define  TEST_WAVE_FILENAME   "D:/data/workspace/C++/DigitalSignalProcessing/FFT/FreeSmallFFT/Resources/WAV/Alarm06.wav"
#define  TEST_WAVE_FILENAME   "D:/data/workspace/C++/DigitalSignalProcessing/FFT/FreeSmallFFT/Resources/WAV/NoiseStereo.wav"




#define     FFT_FREQUENCY_POINTS			 100




// CMainDlg dialog
class CMainDlg : public CDialogEx
{
// Construction
	public:
		CMainDlg(CWnd* pParent = nullptr);	// standard constructor
		~CMainDlg();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RENDERSHAREDMFC_DIALOG };
#endif


protected:
	HICON m_hIcon;

	CBrush  m_backgroundBrush;

	CSpectrumGraphCtrl m_graph;


	CButton m_playButton;
	CButton m_stopButton;
	CButton m_loadWavFileButton;
	CButton m_clearWavFileButton;
	
	CEdit m_wavFileNameEdit;

	CSliderCtrl m_masterVolumeSlider;

	CString m_masterVolumeValue;

	CString m_wavFileNameValue;


	int m_signalType;

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HCURSOR OnQueryDragIcon();

	static UINT run(LPVOID p);


	static UINT runFileLoader(LPVOID p);
	void runFileLoaderThread();	// This is the support method for the run method


	void setupSliders();


	DECLARE_MESSAGE_MAP()


	////////////////////////////////////////////////////////////////////////////  
	// Those functions and variables may be transfered to a WASAPI sound class
	////////////////////////////////////////////////////////////////////////////  

	bool initWASAPI();
	bool terminateWASAPI();
	bool PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole);
	LPWSTR CMainDlg::GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex);
	    
	int loadSignal( int SoundCardChannelCount,
					int wavFileChannelCount,
					CWASAPIRenderer::RenderSampleType SampleType,
					int samplesPerSecond,
					int frameSize,
					int bufferSizeInBytes, int totNumberOfBuffers,
					int filterType);





	bool playTone();

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

	

	int  MasterVolume = 100;
	int  TargetFrequency = 220;
	int  TargetLatency = 50;
	int  TargetDurationInSec = 2;
	bool ShowHelp;
	bool UseConsoleDevice;
	bool UseCommunicationsDevice;
	bool UseMultimediaDevice;
	bool DisableMMCSS;
	int  signalType = 0;
	int  result = 0;

	wchar_t* OutputEndpoint;

	WavLoader* wavLoader = NULL;

	//bool filterSignal = false;
	int  filterType = 0;



public:
	afx_msg void OnNMCustomdrawSineWaveRadio(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedPlayButton();
	afx_msg void OnBnClickedStopButton();
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	
	afx_msg void OnBnClickedLoadWavFile();
	afx_msg void OnBnClickedClearWavFileButtonValue();


	afx_msg void OnBnClickedNoFilterButton();
	afx_msg void OnBnClickedSimpleFilterButton();
	afx_msg void OnBnClickedRunningAverageFilterButton();
	afx_msg void OnBnClickedSmoothOperatorFilterButton();
	afx_msg void OnEnChangeWavFileEdit();

};
