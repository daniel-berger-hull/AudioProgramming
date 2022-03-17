
// WavPlayerFFTDisplay.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWavPlayerFFTDisplayApp:
// See WavPlayerFFTDisplay.cpp for the implementation of this class
//

class CWavPlayerFFTDisplayApp : public CWinApp
{
public:
	CWavPlayerFFTDisplayApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWavPlayerFFTDisplayApp theApp;
