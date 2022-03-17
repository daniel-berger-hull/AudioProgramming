#pragma once
#include <afxwin.h>


/////////////////////////////////////////////////////////////////////////////
// CFrequencyGraph window
enum FrequencyGraphType { FG_MICRO_EQ, FG_EQ_CHANNEL, FG_SPECTRUM, FG_OSCILLOSCOPE, FG_PEAK, FG_PEAK2, FG_PIXELGRAM };


class SpectrumGraphCtrl : public CWnd
{
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
};

