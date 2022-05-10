// CSpectrumGraphCtrl.cpp : implementation file
//

#include "pch.h"
#include "WavPlayerFFTDisplay.h"

#include "CSpectrumGraphCtrl.h"


#include "fft-real-pair.h"




// CSpectrumGraphCtrl

IMPLEMENT_DYNAMIC(CSpectrumGraphCtrl, CStatic)

CSpectrumGraphCtrl ::CSpectrumGraphCtrl()
{
	resetGraphData();

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

	TRACE("MAGNITUDE SOMMATION:\n");
	for (int i = 0; i < SIG_LENGTH; i++)
		TRACE("%i = %f\n", i, magnitudesBuffer[i]);


	if (inputreal != NULL)  delete inputreal;
	if (inputimag != NULL)  delete inputimag;
	if (outputMag != NULL)  delete outputMag;

	if (magnitudesBuffer != NULL)  delete magnitudesBuffer;


	if (rawDataBufferPtr != NULL)  delete rawDataBufferPtr;
	
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

	inputreal        = (double*)malloc(SIG_LENGTH * sizeof(double));
	inputimag        = (double*)malloc(SIG_LENGTH * sizeof(double));
	outputMag        = (double*)malloc(SIG_LENGTH * sizeof(double));
	magnitudesBuffer = (double*)malloc(SIG_LENGTH * sizeof(double));

	
	// The data is stereo, so there 4 + 4 bytes for each samples copied
	rawDataBufferPtr = (BYTE*)malloc(SIG_LENGTH * 8);


	inputreal = zero_reals(inputreal, SIG_LENGTH);
	inputimag = zero_reals(inputimag, SIG_LENGTH);
	outputMag = zero_reals(outputMag, SIG_LENGTH);
	magnitudesBuffer = zero_reals(magnitudesBuffer, SIG_LENGTH);


	for (int i = 0; i < SIG_LENGTH * 8; i++)
		rawDataBufferPtr[i] = 0;

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
	// remove this
	/*if (m_dArray == nullptr)
	{
		return;
	}*/

	if (outputMag == NULL)
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
		//int currentBarHeight = (int)(m_dArray[i] * barMaxHeight);

		int currentBarHeight = (int)((outputMag[i] / 15.0f)* barMaxHeight);
		
		if (currentBarHeight > barMaxHeight) currentBarHeight = barMaxHeight - 1;

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



void CSpectrumGraphCtrl::resetGraphData()
{
	// Should reset the magnitude array created by the FFT intead...
	/*for (int i = 0; i < graphBarCount; i++)
		m_dArray[i] = 0.0;*/
}



void CSpectrumGraphCtrl::setGraphBarCount(int barCount)
{
	//No work if the same number of bar is requested by the client..
	if (barCount == graphBarCount) return;

	// Otherwise, first step is to change the data array size...
	//if (m_dArray != nullptr)  delete m_dArray;

	if (barCount < MIN_GRAPH_BAR_COUNT)  graphBarCount = MIN_GRAPH_BAR_COUNT;
	else if (barCount > MAX_GRAPH_BAR_COUNT)  graphBarCount = MAX_GRAPH_BAR_COUNT;
	else  graphBarCount = barCount;


	//m_dArray = new double[graphBarCount];
	//resetGraphData();
	TRACE("CSpectrumGraphCtrl::setGraphBarCount Bar Count changed to %d bars and data array adjusted accordingly\n", graphBarCount);
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

				//loadData();
				cloneSignalData();
				
				inputimag = zero_reals(inputimag, SIG_LENGTH);
				performFFT();


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





///////////////////////////////////////////////////////////////////////////////////////////////////
//							 FFT Related Functions sections

void CSpectrumGraphCtrl::cloneSignalData()

{

	float* dataBuffer = reinterpret_cast<float*>(rawDataBufferPtr);

	for (int i = 0; i < 256; i++)
	{
		// The orginial signal is stereo, each sample has its left and right data back to back.
		// So data copied by the WASAPIRenderer in the array dataBuffer is 2 times bigger than it should be
		inputreal[i] = dataBuffer[i*2];   // Bring the values from the Generated Signal in Tone.h file...
	}


}


double* CSpectrumGraphCtrl::zero_reals(double* targetBuffer, int n)
{

	for (int i = 0; i < n; i++)
		targetBuffer[i] = 0.0;

	return targetBuffer;
}



void CSpectrumGraphCtrl::magnitudeSummmation(double* dataBuffer, int size)
{

	for (int i = 0; i < size; i++)  
		magnitudesBuffer[i] += dataBuffer[i];

}




void CSpectrumGraphCtrl::performFFT()
{
	
	Fft_transform(inputreal, inputimag, SIG_LENGTH);
	// Get the power of the frequencies distribution, to view it in a graps
	get_output_mag(inputreal, inputimag, outputMag, SIG_LENGTH);


	//Faculative: Add all the magnitude accross all the frequency bins 
	// produced by the FFT. The reason is that we can see a dynamic view
	// of all the frequencies as we go. But we have no idea of the general trend
	// For instance, when analysing a filter, having a view when the frequencies are
	// cut (i.e the summation here will be much lower), this can help a lot 
	magnitudeSummmation(outputMag, SIG_LENGTH);



}


	