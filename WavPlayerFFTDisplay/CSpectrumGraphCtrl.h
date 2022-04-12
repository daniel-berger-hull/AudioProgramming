#pragma once


#define DEFAULT_GRAPH_BAR_COUNT					20
#define MIN_GRAPH_BAR_COUNT						 5
#define MAX_GRAPH_BAR_COUNT					   150


#define REFRESH_EVENTS_COUNT				     2
#define REFRESH_DISPLAY_EVENT_ID				 0
#define	QUIT_REFRESH_DISPLAY_EVENT_ID			 1



#define     SIG_LENGTH							 256
#define     FFT_FREQUENCY_POINTS				 100

// CSpectrumGraphCtrl

class CSpectrumGraphCtrl : public CStatic
{
	DECLARE_DYNAMIC(CSpectrumGraphCtrl)

public:
	CSpectrumGraphCtrl();
	virtual ~CSpectrumGraphCtrl();

	afx_msg void OnPaint();


	bool startDisplayRefresh();
	void stopDisplayRefresh();

	int   getGraphBarCount()				{ return graphBarCount; }
	void  setGraphBarCount(int barCount);
	
	HANDLE  getRefreshEvent()		{ return ghEvents[REFRESH_DISPLAY_EVENT_ID]; }
	void    setRefreshEvent();
	
	BYTE* getRawDataBufferPtr()		{ return rawDataBufferPtr; }

//	void loadData();

	
protected:

	int graphBarCount = DEFAULT_GRAPH_BAR_COUNT;
	//double* m_dArray = nullptr;

	/// FFT Related arrays...
	double* inputreal = NULL;
	double* inputimag = NULL;
	double* outputMag = NULL;

	// This buffer is used by the WASAPI to make copy of its data 
	// it currenlty sending to the sound card, and is going to be used
	// to perform an FFT on this data to display on the bar graph
	BYTE* rawDataBufferPtr = NULL;



	void resetGraphData();

	/// FFT Related Function...
	void    cloneSignalData();
	double* zero_reals(double* targetBuffer, int n);
	void    performFFT();



	void drawGraph(CPaintDC* dc, CRect* controlRect);

	static UINT run(LPVOID p);
	void   displayRefreshThread();	// This is the support method for the run method
	// 2 events used to drive the refresh Thread
	//  First is a refresh display Event
	//  Second is terminate the refresh thread (App is terminating)
	HANDLE  ghEvents[2];			

	bool runRefreshDiplay = FALSE;

	DECLARE_MESSAGE_MAP()


	virtual void PreSubclassWindow();
};


