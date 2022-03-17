// CSpectrumGraphCtrl.cpp : implementation file
//

#include "pch.h"
#include "WavPlayerFFTDisplay.h"
#include "CSpectrumGraphCtrl.h"


// CSpectrumGraphCtrl

IMPLEMENT_DYNAMIC(CSpectrumGraphCtrl, CStatic)

CSpectrumGraphCtrl::CSpectrumGraphCtrl()
{
	m_dArray = new double[graphBarCount];

	for (int i = 0; i < REFRESH_EVENTS_COUNT; i++)
	{
		ghEvents[i] = CreateEvent(
			NULL,   // default security attributes
			FALSE,  // auto-reset event object
			FALSE,  // initial state is nonsignaled
			NULL);  // unnamed object

		if (ghEvents[i] == NULL)
		{
			printf("CreateEvent error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}


}

CSpectrumGraphCtrl::~CSpectrumGraphCtrl()
{
	if (m_dArray != nullptr)  delete m_dArray;

	for (int i = 0; i < REFRESH_EVENTS_COUNT; i++)
		CloseHandle(ghEvents[i]);
}


BEGIN_MESSAGE_MAP(CSpectrumGraphCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////////////////////////////////// 
//					CSpectrumGraphCtrl message handlers


void CSpectrumGraphCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	//Make sure that there are at least some data, otherwise the OnPaint will crash
	loadData();
	AfxBeginThread(run, this);
}


void CSpectrumGraphCtrl::OnPaint()
{

	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages
	CRect ctrlRect, currentOutsideBarRect, currentInnerBarRect;
	GetClientRect(&ctrlRect);
	CBrush br(RGB(0, 0, 0));
	dc.FillRect(&ctrlRect, &br);


	drawGraph(&dc, &ctrlRect);
}

//////////////////////////////////////////////////////////////////////////////////////////// 
//					CSpectrumGraphCtrl Utility Functions


void CSpectrumGraphCtrl::drawGraph(CPaintDC* dc, CRect* controlRect)
{

	if (m_dArray == nullptr)
	{
		return;
	}

	CRect ctrlRect, currentOutsideBarRect, currentInnerBarRect;

	int barWidth = (controlRect->right - controlRect->left) / getGraphBarCount();
	int barMaxHeight = controlRect->bottom - controlRect->top;

	currentOutsideBarRect.left = controlRect->left;
	currentOutsideBarRect.right = currentOutsideBarRect.left + barWidth;
	currentOutsideBarRect.bottom = controlRect->bottom;

	currentInnerBarRect.left = currentOutsideBarRect.left + 1;
	currentInnerBarRect.right = currentOutsideBarRect.right - 1;
	currentInnerBarRect.bottom = currentOutsideBarRect.bottom - 1;


	CBrush barBrush(RGB(0, 255, 0));
	CPen pen(PS_SOLID, 1, RGB(0, 170, 0));


	dc->SelectObject(pen);
	for (int i = 0; i < getGraphBarCount(); i++)
	{
		int currentBarHeight = (int)(m_dArray[i] * barMaxHeight);
		
		currentOutsideBarRect.top = controlRect->bottom - currentBarHeight;
		currentInnerBarRect.top = currentOutsideBarRect.top + 1;

		dc->Rectangle(&currentOutsideBarRect);
		dc->FillRect(&currentInnerBarRect, &barBrush);

		currentOutsideBarRect.left += barWidth;
		currentOutsideBarRect.right = currentOutsideBarRect.left + barWidth;
		currentInnerBarRect.left = currentOutsideBarRect.left + 1;
		currentInnerBarRect.right = currentOutsideBarRect.right - 1;
	}

}




void CSpectrumGraphCtrl::setGraphBarCount(int barCount)
{
	if (barCount < MIN_GRAPH_BAR_COUNT)  graphBarCount = MIN_GRAPH_BAR_COUNT;
	else if (barCount > MAX_GRAPH_BAR_COUNT)  graphBarCount = MAX_GRAPH_BAR_COUNT;
	else  graphBarCount = barCount;
}





// This function is not used: Should it be deleted?
bool CSpectrumGraphCtrl::startDisplayRefresh()
{
	return TRUE;

}

void CSpectrumGraphCtrl::stopDisplayRefresh()
{
	TRACE("Will set the QUIT_REFRESH_DISPLAY_EVENT_ID event...\n");
	SetEvent(this->ghEvents[QUIT_REFRESH_DISPLAY_EVENT_ID]);
}



UINT CSpectrumGraphCtrl::run(LPVOID p)
{

	TRACE(" CSpectrumGraphCtrl::run(LPVOID p) static version...\n");
	CSpectrumGraphCtrl* me = (CSpectrumGraphCtrl*)p;
	me->displayRefreshThread();

	return 0;
}


void CSpectrumGraphCtrl::displayRefreshThread()
{

	TRACE(" CSpectrumGraphCtrl::runThread() started..\n");
	runRefreshDiplay = TRUE;
	int threadTimeOut = 0;

	DWORD dwEvent;
	

	while (runRefreshDiplay)
	{
		dwEvent = WaitForMultipleObjects(REFRESH_EVENTS_COUNT,           // number of objects in array
										 ghEvents,     // array of objects
										 FALSE,       // wait for any object
										 INFINITE);       // five-second wait


		switch (dwEvent)
		{
				// ghEvents[0] (Refresh Display Request) was signaled
			case WAIT_OBJECT_0 + 0:
				//TRACE(" CSpectrumGraphCtrl::runThread() First event was signaled..\n");
				Invalidate(TRUE);
				break;

				// ghEvents[1] (Terminate Refresh Thread) was signaled
			case WAIT_OBJECT_0 + 1:
				TRACE(" CSpectrumGraphCtrl::runThread() Second event was signaled..\n");
				runRefreshDiplay = FALSE;
				break;
		}
	}

	TRACE(" CSpectrumGraphCtrl::runThread() end..\n");
}




void CSpectrumGraphCtrl::setRefreshEvent() 
{
	SetEvent(this->ghEvents[REFRESH_DISPLAY_EVENT_ID]);
}


// This method puts dummy data for now...
// The result of an FFT should be used here instead...
void CSpectrumGraphCtrl::loadData()
{
	
	for (int i = 0; i < graphBarCount; i++)
	{
		m_dArray[i] = (rand() / (double)RAND_MAX);
	}
	
}


	