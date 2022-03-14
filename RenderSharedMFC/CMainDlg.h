
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



////////////////////////////////////////////////////////////////////////////  


#define MINIMAL_FREQUENCY                 220
#define MAXMIMAL_FREQUENCY                880
#define MINIMAL_VOLUME                      0
#define MAXMIMAL_VOLUME                    10

#define HARMONIC_MIN_LEVEL				    0
#define HARMONIC_MAX_LEVEL				  100


#define  SINE_SIGNAL                         1
#define  COMPLEX_SINE_SIGNAL                 2
#define  TRIANGLE_SIGNAL                     3
#define  SAW_SIGNAL                          4
#define  SQUARE_SIGNAL                       5
#define  NOISE_SIGNAL						 6
#define  COMPLEX_SIGNAL						 7


#define  WHITE_NOISE_SIGNAL					  10
#define  PINK_NOISE_SIGNAL					  11
#define  BROWN_NOISE_SIGNAL                   12

////////////////////////////////////////////////////////////////////////////  
// Those declaration may be transfered to a WASAPI sound class
////////////////////////////////////////////////////////////////////////////  

//typedef struct PlayThreadParams {
//	int frequency;
//	int durationInSec;
//} PLAY_THREAD_PARAMS, * PPLAY_THREAD_PARAMS;
//
//PPLAY_THREAD_PARAMS params = NULL;


class CThreadParamObject : public CObject
{


	public:
		CThreadParamObject() : frequency(0), durationInSec(0)			{ }
		CThreadParamObject(int f, int d): frequency(f),durationInSec(d)	{ }

		int getFrequency()       { return frequency; }
		int getDurationInSec()   { return durationInSec; }

	private:
	int frequency;
	int durationInSec;

};




////////////////////////////////////////////////////////////////////////////  


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

	CButton m_playButton;
	CButton m_stopButton;

	CButton m_addNoiseCheckBox;
	CButton m_sineWaveType;
	CButton m_whiteNoiseType;
	CButton m_pinkNoiseType;
	CButton m_brownNoiseType;

	CSliderCtrl m_frequencySignalSlider;
	CSliderCtrl m_masterVolumeSlider;
	CSliderCtrl m_firstHarmonicLevelSlider;
	CSliderCtrl m_secondHarmonicLevelSlider;
	CSliderCtrl m_thirdHarmonicLevelSlider;
	CSliderCtrl m_fourthHarmonicLevelSlider;


	CStatic m_frequencyLabel;

	CStatic m_harmonicLevelsGroup;
	CString m_frequencyValue;
	CString m_masterVolumeValue;

	CString m_firstHarmonicLevel;
	CString m_secondHarmonicLevel;
	CString m_thirdHarmonicLevel;
	CString m_fourthHarmonicLevel;

	int m_signalType;
	int m_noiseType;

	bool addNoiseIndicator = false;

	int m_harmonicLevels[4];

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HCURSOR OnQueryDragIcon();

	static UINT run(LPVOID p);

	void decorateRadioButtonSection();
	void setupSliders();
	void updateSliders();

	void EnableHarmonicLevels(BOOL bEnable);
	void EnableNoiseTypeSection(BOOL bEnable);

	void updateHarmonicLevels(int harmonicIndex, int value);

	DECLARE_MESSAGE_MAP()


	////////////////////////////////////////////////////////////////////////////  
	// Those functions and variables may be transfered to a WASAPI sound class
	////////////////////////////////////////////////////////////////////////////  

	bool initWASAPI();
	bool terminateWASAPI();
	bool PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole);
	LPWSTR CMainDlg::GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex);
	int generateSignal(int signalFreq, int channelCount,
						CWASAPIRenderer::RenderSampleType SampleType,
						int samplesPerSecond,
						int frameSize,
						int bufferSizeInBytes, int totNumberOfBuffers,bool addNoiseIndicator);
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
	int  signalType = SINE_SIGNAL;
	int  result = 0;

	wchar_t* OutputEndpoint;

	////////////////////////////////////////////////////////////////////  

public:
	afx_msg void OnNMCustomdrawSineWaveRadio(NMHDR* pNMHDR, LRESULT* pResult);
	
	
	afx_msg void OnBnClickedPlayButton();
	afx_msg void OnBnClickedStopButton();
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSineWaveRadio();
	afx_msg void OnBnClickedSawWaveRadio();
	afx_msg void OnBnClickedTriangleWaveRadio();
	afx_msg void OnBnClickedSquareWaveRadio();
	afx_msg void OnBnClickedNoiseWaveRadio();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedComplexSignalRadio();	
	afx_msg void OnBnClickedAddNoiseCheck();

	afx_msg void OnBnClickedWhiteNoiseTypeRadio();
	afx_msg void OnBnClickedPinkNoiseTypeRadio();
	afx_msg void OnBnClickedBrownNoiseTypeRadio();
};
