#pragma once


#define DEFAULT_GRAPH_BAR_COUNT		20
#define MIN_GRAPH_BAR_COUNT			 5
#define MAX_GRAPH_BAR_COUNT			50


#define REFRESH_EVENTS_COUNT					     2
#define REFRESH_DISPLAY_EVENT_ID				 0
#define	QUIT_REFRESH_DISPLAY_EVENT_ID			 1



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
	
	void setRefreshEvent();
	

	void loadData();

	
protected:

	int graphBarCount = DEFAULT_GRAPH_BAR_COUNT;
	double* m_dArray = nullptr;


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


