
// CMainDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RenderSharedMFC.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include "ToneGen.h"

#include "PinkNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainDlg dialog


typedef struct PlayThreadParams {
	int frequency;
	int durationInSec;
} PLAY_THREAD_PARAMS, * PPLAY_THREAD_PARAMS;






WaveParams  currentWaveParams;

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RENDERSHAREDMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMainDlg::~CMainDlg()
{
	if (pParamObject != NULL)  delete pParamObject;
}



void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PLAY_BUTTON, m_playButton);
	DDX_Control(pDX, IDC_STOP_BUTTON, m_stopButton);

	DDX_Control(pDX, IDC_SIGNAL_FREQUENCY_SLIDER, m_frequencySignalSlider);
	DDX_Control(pDX, IDC_MASTER_VOLUME_SLIDER, m_masterVolumeSlider);

	DDX_Control(pDX, IDC_BASE_HARMONIC_LEVEL_SLIDER, m_firstHarmonicLevelSlider);
	DDX_Control(pDX, IDC_SECOND_HARMONIC_LEVEL_SLIDER, m_secondHarmonicLevelSlider);
	DDX_Control(pDX, IDC_THIRD_HARMONIC_LEVEL_SLIDER, m_thirdHarmonicLevelSlider);
	DDX_Control(pDX, IDC_FOURTH_HARMONIC_LEVEL_SLIDER, m_fourthHarmonicLevelSlider);


	DDX_Text(pDX, IDC_SIGNAL_FREQUENCY_LABEL, m_frequencyValue);
	DDX_Text(pDX, IDC_MASTER_VOLUME_LABEL, m_masterVolumeValue);

	DDX_Text(pDX, IDC_BASE_HARMONIC_LEVEL, m_firstHarmonicLevel);
	DDX_Text(pDX, IDC_SECOND_HARMONIC_LEVEL, m_secondHarmonicLevel);
	DDX_Text(pDX, IDC_THIRD_HARMONIC_LEVEL, m_thirdHarmonicLevel);
	DDX_Text(pDX, IDC_FOURTH_HARMONIC_LEVEL, m_fourthHarmonicLevel);

	DDX_Control(pDX, IDC_ADD_NOISE_CHECK, m_addNoiseCheckBox);
	DDX_Control(pDX, IDC_SINE_WAVE_RADIO, m_sineWaveType);
	DDX_Control(pDX, IDC_WHITE_NOISE_TYPE_RADIO, m_whiteNoiseType);
	DDX_Control(pDX, IDC_PINK_NOISE_TYPE_RADIO, m_pinkNoiseType);
	DDX_Control(pDX, IDC_BROWN_NOISE_TYPE_RADIO, m_brownNoiseType);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()

	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SINE_WAVE_RADIO, &CMainDlg::OnNMCustomdrawSineWaveRadio)

	ON_BN_CLICKED(IDC_PLAY_BUTTON, &CMainDlg::OnBnClickedPlayButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CMainDlg::OnBnClickedStopButton)
	ON_BN_CLICKED(IDC_SINE_WAVE_RADIO, &CMainDlg::OnBnClickedSineWaveRadio)
	ON_BN_CLICKED(IDC_SAW_WAVE_RADIO, &CMainDlg::OnBnClickedSawWaveRadio)
	ON_BN_CLICKED(IDC_TRIANGLE_WAVE_RADIO, &CMainDlg::OnBnClickedTriangleWaveRadio)
	ON_BN_CLICKED(IDC_SQUARE_WAVE_RADIO, &CMainDlg::OnBnClickedSquareWaveRadio)
	ON_BN_CLICKED(IDC_NOISE_WAVE_RADIO, &CMainDlg::OnBnClickedNoiseWaveRadio)
	ON_BN_CLICKED(IDC_COMPLEX_SIGNAL_RADIO, &CMainDlg::OnBnClickedComplexSignalRadio)
	ON_BN_CLICKED(IDC_ADD_NOISE_CHECK, &CMainDlg::OnBnClickedAddNoiseCheck)
	ON_BN_CLICKED(IDC_WHITE_NOISE_TYPE_RADIO, &CMainDlg::OnBnClickedWhiteNoiseTypeRadio)
	ON_BN_CLICKED(IDC_PINK_NOISE_TYPE_RADIO, &CMainDlg::OnBnClickedPinkNoiseTypeRadio)
	ON_BN_CLICKED(IDC_BROWN_NOISE_TYPE_RADIO, &CMainDlg::OnBnClickedBrownNoiseTypeRadio)
END_MESSAGE_MAP()


// CMainDlg message handlers





static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

void CMainDlg::setupSliders()
{
	m_frequencySignalSlider.SetRange(MINIMAL_FREQUENCY, MAXMIMAL_FREQUENCY); //sending true\false doesn't matter
	m_frequencySignalSlider.SetPos(MINIMAL_FREQUENCY);
	m_frequencySignalSlider.SetTic(20);
	m_frequencySignalSlider.SetTicFreq(1);

	m_masterVolumeSlider.SetRange(0, 100);
	m_masterVolumeSlider.SetPos(100);


	m_firstHarmonicLevelSlider.SetRange(HARMONIC_MIN_LEVEL, HARMONIC_MAX_LEVEL); //sending true\false doesn't matter
	m_firstHarmonicLevelSlider.SetPos(50);

	m_secondHarmonicLevelSlider.SetRange(HARMONIC_MIN_LEVEL, HARMONIC_MAX_LEVEL); //sending true\false doesn't matter
	m_secondHarmonicLevelSlider.SetPos(30);

	m_thirdHarmonicLevelSlider.SetRange(HARMONIC_MIN_LEVEL, HARMONIC_MAX_LEVEL); //sending true\false doesn't matter
	m_thirdHarmonicLevelSlider.SetPos(15);

	m_fourthHarmonicLevelSlider.SetRange(HARMONIC_MIN_LEVEL, HARMONIC_MAX_LEVEL); //sending true\false doesn't matter
	m_fourthHarmonicLevelSlider.SetPos(5);

	m_frequencyValue.Format(_T("%d"), MINIMAL_FREQUENCY);
	m_masterVolumeValue.Format(_T("%d"), 100);
}



void CMainDlg::decorateRadioButtonSection()
{
	HBITMAP sineBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SINE_WAVE));
	HBITMAP sawBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SAW_WAVE));
	HBITMAP triangleBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_TRIANGLE_WAVE));
	HBITMAP squareBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SQUARE_WAVE));
	HBITMAP noiseBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_NOISE_WAVE));

	CWnd* sineRadioButton = GetDlgItem(IDC_SINE_WAVE_RADIO);
	CWnd* sawRadioButton = GetDlgItem(IDC_SAW_WAVE_RADIO);
	CWnd* triangleRadioButton = GetDlgItem(IDC_TRIANGLE_WAVE_RADIO);
	CWnd* squareRadioButton = GetDlgItem(IDC_SQUARE_WAVE_RADIO);
	CWnd* noiseRadioButton = GetDlgItem(IDC_NOISE_WAVE_RADIO);

	sineRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)sineBitmap);
	sawRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)sawBitmap);
	triangleRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)triangleBitmap);
	squareRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)squareBitmap);
	noiseRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)noiseBitmap);
}



BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	m_backgroundBrush.CreateSolidBrush(RGB(0, 84, 165));


	setupSliders();

	
	
	decorateRadioButtonSection();

	m_signalType = SINE_SIGNAL;


	m_harmonicLevels[0] = 50;
	m_harmonicLevels[1] = 30;
	m_harmonicLevels[2] = 15;
	m_harmonicLevels[3] = 5;
	

	m_firstHarmonicLevel.Format(_T("%d"), m_harmonicLevels[0]);
	m_secondHarmonicLevel.Format(_T("%d"), m_harmonicLevels[1]);
	m_thirdHarmonicLevel.Format(_T("%d"), m_harmonicLevels[2]);
	m_fourthHarmonicLevel.Format(_T("%d"), m_harmonicLevels[3]);




	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}




HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);


	if (nCtlColor == CTLCOLOR_STATIC )
	{
		if (pWnd->GetDlgCtrlID() == IDC_SINE_WAVE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_SAW_WAVE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_TRIANGLE_WAVE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_SQUARE_WAVE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_NOISE_WAVE_RADIO || 
			pWnd->GetDlgCtrlID() == IDC_COMPLEX_SIGNAL_RADIO)
		{
			pDC->SetTextColor(RGB(255, 255, 255));

			return m_backgroundBrush;
		}
		
		if (pWnd->GetDlgCtrlID() == IDC_SIGNAL_FREQUENCY_SLIDER ||
			pWnd->GetDlgCtrlID() == IDC_MASTER_VOLUME_SLIDER || 
			pWnd->GetDlgCtrlID() == IDC_BASE_HARMONIC_LEVEL_SLIDER ||
			pWnd->GetDlgCtrlID() == IDC_SECOND_HARMONIC_LEVEL_SLIDER ||
			pWnd->GetDlgCtrlID() == IDC_THIRD_HARMONIC_LEVEL_SLIDER ||
			pWnd->GetDlgCtrlID() == IDC_FOURTH_HARMONIC_LEVEL_SLIDER )
		{
			//pDC->SetTextColor(RGB(255, 255, 255));

			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}

		if (pWnd->GetDlgCtrlID() == IDC_SIGNAL_FREQUENCY_LABEL ||
			pWnd->GetDlgCtrlID() == IDC_FREQUENCY_HZ_LABEL ||
			pWnd->GetDlgCtrlID() == IDC_MASTER_VOLUME_LABEL ||
			pWnd->GetDlgCtrlID() == IDC_BASE_HARMONIC_LEVEL ||
			pWnd->GetDlgCtrlID() == IDC_SECOND_HARMONIC_LEVEL ||
			pWnd->GetDlgCtrlID() == IDC_THIRD_HARMONIC_LEVEL ||
			pWnd->GetDlgCtrlID() == IDC_FOURTH_HARMONIC_LEVEL )
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush; 
		}

		

		

		if (pWnd->GetDlgCtrlID() == IDC_GROUP_VOLUME ||
			pWnd->GetDlgCtrlID() == IDC_GROUP_FREQUENCY ||
			pWnd->GetDlgCtrlID() == IDC_WAVE_TYPE_STATIC ||
			pWnd->GetDlgCtrlID() == IDC_HARMONIC_LEVEL_GROUP )
		{
			pDC->SetTextColor(RGB(255,255,255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}


		if (pWnd->GetDlgCtrlID() == IDC_NOISE_TYPE_GROUP ||
			pWnd->GetDlgCtrlID() == IDC_WHITE_NOISE_TYPE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_PINK_NOISE_TYPE_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_BROWN_NOISE_TYPE_RADIO)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}

		if (pWnd->GetDlgCtrlID() == IDC_ADD_NOISE_CHECK)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}

		

	}
	
	return hbr;
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		CPaintDC dc(this); // device context for painting
		CRect rect;
			
		GetClientRect(&rect);
		dc.FillRect(&rect, &m_backgroundBrush);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMainDlg::OnNMCustomdrawSineWaveRadio(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	*pResult = 0;
}


void CMainDlg::OnBnClickedPlayButton()
{
	playTone(TargetFrequency, TargetDurationInSec);
}


void CMainDlg::OnBnClickedStopButton()
{
	// TODO: Add your control notification handler code here
}


void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == (CScrollBar*)&m_frequencySignalSlider)
	{
		int value = m_frequencySignalSlider.GetPos();
		m_frequencyValue.Format(_T("%d"), value);
		TargetFrequency = value;

		UpdateData(FALSE);
	}
	else if (pScrollBar == (CScrollBar*)&m_masterVolumeSlider)
	{
		int value = m_masterVolumeSlider.GetPos();
		MasterVolume = value;

		m_masterVolumeValue.Format(_T("%d"), value);
		UpdateData(FALSE);
	}
	else if (pScrollBar == (CScrollBar*)&m_firstHarmonicLevelSlider)
	{
		int value = m_firstHarmonicLevelSlider.GetPos();
		m_harmonicLevels[0] = value;
	
		m_firstHarmonicLevel.Format(_T("%d"), value);

		updateHarmonicLevels(0, value);
		updateSliders();
		UpdateData(FALSE);
	}
	else if (pScrollBar == (CScrollBar*)&m_secondHarmonicLevelSlider)
	{
		int value = m_secondHarmonicLevelSlider.GetPos();
		
		m_harmonicLevels[1] = value;
		m_secondHarmonicLevel.Format(_T("%d"), value);
		updateHarmonicLevels(1, value);
		updateSliders();

		UpdateData(FALSE);
	}
	else if (pScrollBar == (CScrollBar*)&m_thirdHarmonicLevelSlider)
	{
		int value = m_thirdHarmonicLevelSlider.GetPos();
		
		m_harmonicLevels[2] = value;
		m_thirdHarmonicLevel.Format(_T("%d"), value);
		updateHarmonicLevels(2, value);
		updateSliders();

		UpdateData(FALSE);
	}
	else if (pScrollBar == (CScrollBar*)&m_fourthHarmonicLevelSlider)
	{
		int value = m_fourthHarmonicLevelSlider.GetPos();
		
		m_harmonicLevels[3] = value;
		m_fourthHarmonicLevel.Format(_T("%d"), value);
		updateHarmonicLevels(3, value);
		updateSliders();

		UpdateData(FALSE);
	}	
	else {
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}




void CMainDlg::OnBnClickedSineWaveRadio()
{
	m_signalType = SINE_SIGNAL;
	EnableHarmonicLevels(FALSE);
	EnableNoiseTypeSection(FALSE);
}


void CMainDlg::OnBnClickedSawWaveRadio()
{
	m_signalType = SAW_SIGNAL;
	EnableHarmonicLevels(FALSE);
	EnableNoiseTypeSection(FALSE);
}


void CMainDlg::OnBnClickedTriangleWaveRadio()
{
	m_signalType = TRIANGLE_SIGNAL;
	EnableHarmonicLevels(FALSE);
	EnableNoiseTypeSection(FALSE);
}


void CMainDlg::OnBnClickedSquareWaveRadio()
{
	m_signalType = SQUARE_SIGNAL;
	EnableHarmonicLevels(FALSE);
	EnableNoiseTypeSection(FALSE);
}


void CMainDlg::OnBnClickedNoiseWaveRadio()
{
	m_signalType = NOISE_SIGNAL;

	EnableNoiseTypeSection(TRUE);
	EnableHarmonicLevels(FALSE);
}

void CMainDlg::OnBnClickedComplexSignalRadio()
{
	m_signalType = COMPLEX_SIGNAL;
	EnableHarmonicLevels(TRUE);
	EnableNoiseTypeSection(FALSE);
}




void CMainDlg::OnBnClickedAddNoiseCheck()
{
	if (addNoiseIndicator)  addNoiseIndicator = false;
	else addNoiseIndicator = true;
}


void CMainDlg::OnBnClickedWhiteNoiseTypeRadio()
{
	m_noiseType = WHITE_NOISE_SIGNAL;
}


void CMainDlg::OnBnClickedPinkNoiseTypeRadio()
{
	m_noiseType = PINK_NOISE_SIGNAL;
}


void CMainDlg::OnBnClickedBrownNoiseTypeRadio()
{
	m_noiseType = BROWN_NOISE_SIGNAL;
}

void CMainDlg::EnableHarmonicLevels(BOOL bEnable)
{
	m_firstHarmonicLevelSlider.EnableWindow(bEnable);
	m_secondHarmonicLevelSlider.EnableWindow(bEnable);
	m_thirdHarmonicLevelSlider.EnableWindow(bEnable);
	m_fourthHarmonicLevelSlider.EnableWindow(bEnable);
}

void CMainDlg::EnableNoiseTypeSection(BOOL bEnable)
{
	m_whiteNoiseType.EnableWindow(bEnable);
	m_pinkNoiseType.EnableWindow(bEnable);
	m_brownNoiseType.EnableWindow(bEnable);
}




void CMainDlg::updateSliders()
{
	m_firstHarmonicLevelSlider.SetPos((int)(currentWaveParams.amplitude[0] * 100.0f));
	m_secondHarmonicLevelSlider.SetPos((int)(currentWaveParams.amplitude[1] * 100.0f));
	m_thirdHarmonicLevelSlider.SetPos((int)(currentWaveParams.amplitude[2] * 100.0f));
	m_fourthHarmonicLevelSlider.SetPos((int)(currentWaveParams.amplitude[3] * 100.0f));

}






int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	initWASAPI();


	return 0;
}


void CMainDlg::OnDestroy()
{

	terminateWASAPI();

	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
}


UINT CMainDlg::run(LPVOID p)
{

	TRACE(" CListSizeDraw::run(LPVOID p) static version...\n");
	CMainDlg* me = (CMainDlg*)p;
	me->PlayToneThreadProc(NULL);

	
	return 0;
}




////////////////////////////////////////////////////////////////////  
// Those functions below may be transfered to a WASAPI sound class
////////////////////////////////////////////////////////////////////  


bool CMainDlg::initWASAPI()
{
	TRACE("Initialisation of WASAPI...\n");
	//
  //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
  //
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		TRACE("Unable to initialize COM: %x\n", hr);
		result = hr;
		return false;
	}
	TRACE("CoInitializeEx using CoInitializeEx\n");

	//
	//  Now that we've parsed our command line, pick the device to render.
	//
	if (!PickDevice(&device, &isDefaultDevice, &role))
	{
		result = -1;
		return false;
	}
	TRACE("Device picked successfully\n");

	renderer = new CWASAPIRenderer(device, isDefaultDevice, role);


	if (renderer == NULL)
	{
		TRACE("Unable to allocate renderer\n");
		false;
	}
	TRACE("CWASAPIRenderer instance created\n");



	if (renderer->Initialize(TargetLatency))
	{
		renderBufferSizeInBytes = (renderer->BufferSizePerPeriod() * renderer->FrameSize());
		renderDataLength = (renderer->SamplesPerSecond() * TargetDurationInSec * renderer->FrameSize()) + (renderBufferSizeInBytes - 1);
		renderBufferCount = renderDataLength / (renderBufferSizeInBytes);

		TRACE("CWASAPIRenderer initiated with success\n");
		TRACE("Buffer Size per Period  = %d\n", renderer->BufferSizePerPeriod());
		TRACE("Frame Size = %d\n", renderer->FrameSize());

		TRACE("Buffer Size In Bytes = %d\n", renderBufferSizeInBytes);
		TRACE("Data Length = %d\n", renderBufferSizeInBytes);
		TRACE("Total number of Buffers = %d\n", renderBufferCount);
	}
	else
	{
		TRACE("CWASAPIRenderer initialisation failed!\n");
		return false;
	}

	TRACE("WASAPI Initialisation is completed\n");

	return true;
}

bool CMainDlg::terminateWASAPI()
{
	TRACE("Will Terminate of WASAPI...\n");

	if (renderer != NULL)
	{
		renderer->Shutdown();
		SafeRelease(&renderer);
		TRACE("Released the CWASAPIRenderer class\n");
	}

	SafeRelease(&device);
	CoUninitialize();
	TRACE("WASAPI is realeased and terminated\n");
	return true;
}


bool CMainDlg::PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole)
{
	HRESULT hr;
	bool retValue = true;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;

	*IsDefaultDevice = false;   // Assume we're not using the default device.

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		TRACE("Unable to instantiate device enumerator: %x\n", hr);
		retValue = false;
		goto Exit;
	}

	IMMDevice* device = NULL;

	//
	//  First off, if none of the console switches was specified, use the console device.
	//
	if (!UseConsoleDevice && !UseCommunicationsDevice && !UseMultimediaDevice && OutputEndpoint == NULL)
	{
		//
		//  The user didn't specify an output device, prompt the user for a device and use that.
		//
		hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
		if (FAILED(hr))
		{
			TRACE("Unable to retrieve device collection: %x\n", hr);
			retValue = false;
			goto Exit;
		}

		TRACE("Available an output device(s):\n");
		TRACE("    0:  Default Console Device\n");
		TRACE("    1:  Default Communications Device\n");
		TRACE("    2:  Default Multimedia Device\n");
		UINT deviceCount;
		hr = deviceCollection->GetCount(&deviceCount);
		if (FAILED(hr))
		{
			TRACE("Unable to get device collection length: %x\n", hr);
			retValue = false;
			goto Exit;
		}
		for (UINT i = 0; i < deviceCount; i += 1)
		{
			LPWSTR deviceName;

			deviceName = GetDeviceName(deviceCollection, i);
			if (deviceName == NULL)
			{
				retValue = false;
				goto Exit;
			}
			TRACE("    %d:  %S\n", i + 3, deviceName);
			free(deviceName);
		}
		wchar_t choice[10];
		// _getws_s(choice);   // Note: Using the safe CRT version of _getws.

		 //long deviceIndex;
		 //wchar_t* endPointer;


		 //deviceIndex = wcstoul(choice, &endPointer, 0);
		wchar_t* endPointer = choice;
		long deviceIndex = 3;


		if (deviceIndex == 0 && endPointer == choice)
		{
			TRACE("unrecognized device index: %S\n", choice);
			retValue = false;
			goto Exit;
		}
		switch (deviceIndex)
		{
		case 0:
			UseConsoleDevice = 1;
			break;
		case 1:
			UseCommunicationsDevice = 1;
			break;
		case 2:
			UseMultimediaDevice = 1;
			break;
		default:
			hr = deviceCollection->Item(deviceIndex - 3, &device);
			if (FAILED(hr))
			{
				TRACE("Unable to retrieve device %d: %x\n", deviceIndex - 3, hr);
				retValue = false;
				goto Exit;
			}
			break;
		}
	}
	else if (OutputEndpoint != NULL)
	{
		hr = deviceEnumerator->GetDevice(OutputEndpoint, &device);
		if (FAILED(hr))
		{
			TRACE("Unable to get endpoint for endpoint %S: %x\n", OutputEndpoint, hr);
			retValue = false;
			goto Exit;
		}
	}

	if (device == NULL)
	{
		ERole deviceRole = eConsole;    // Assume we're using the console role.
		if (UseConsoleDevice)
		{
			deviceRole = eConsole;
		}
		else if (UseCommunicationsDevice)
		{
			deviceRole = eCommunications;
		}
		else if (UseMultimediaDevice)
		{
			deviceRole = eMultimedia;
		}
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &device);
		if (FAILED(hr))
		{
			TRACE("Unable to get default device for role %d: %x\n", deviceRole, hr);
			retValue = false;
			goto Exit;
		}
		*IsDefaultDevice = true;
		*DefaultDeviceRole = deviceRole;
	}

	*DeviceToUse = device;
	retValue = true;
Exit:
	SafeRelease(&deviceCollection);
	SafeRelease(&deviceEnumerator);

	return retValue;
}



LPWSTR CMainDlg::GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex)
{
	IMMDevice* device;
	LPWSTR deviceId;
	HRESULT hr;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
	{
		TRACE("Unable to get device %d: %x\n", DeviceIndex, hr);
		return NULL;
	}
	hr = device->GetId(&deviceId);
	if (FAILED(hr))
	{
		TRACE("Unable to get device %d id: %x\n", DeviceIndex, hr);
		return NULL;
	}

	IPropertyStore* propertyStore;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	SafeRelease(&device);
	if (FAILED(hr))
	{
		TRACE("Unable to open device %d property store: %x\n", DeviceIndex, hr);
		return NULL;
	}

	PROPVARIANT friendlyName;
	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	SafeRelease(&propertyStore);

	if (FAILED(hr))
	{
		TRACE("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	wchar_t deviceName[128];
	hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
	if (FAILED(hr))
	{
		TRACE("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	PropVariantClear(&friendlyName);
	CoTaskMemFree(deviceId);

	wchar_t* returnValue = _wcsdup(deviceName);
	if (returnValue == NULL)
	{
		TRACE("Unable to allocate buffer for return\n");
		return NULL;
	}
	return returnValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//   signalFreq:         desired frequency of the signal to produce
//   channelCount:       Can be either 1 (mono) or 2 (Stereo)
//   SampleType:        WASAPI related values  --> SampleTypeFloat or SampleType16BitPCM
//   samplesPerSecond:   Number of samples by second to provide to the sound card
//   frameSize:          Size of one full sample:  BitPerSample * channelCount
//   bufferSizeInBytes:  Full size of the byte array to contains all the frames data
//   totNumberOfBuffer:  how many sub buffer required to cover all the time of the signal
int CMainDlg::generateSignal(int signalFreq, int channelCount,
	CWASAPIRenderer::RenderSampleType SampleType,
	int samplesPerSecond,
	int frameSize,
	int bufferSizeInBytes, int totNumberOfBuffers,
	bool addNoiseIndicator)
{
	RenderBuffer** currentBufferTail = &renderQueue;

	double theta = 0.0;
	double carryOver = 0.0;

	GenParams genParams;


	//genParams.Buffer = renderBuffer->_Buffer;
	//genParams.BufferLength = renderBuffer->_BufferLength;
	genParams.Frequency = TargetFrequency;
	genParams.ChannelCount = channelCount;
	genParams.SamplesPerSecond = samplesPerSecond;
	genParams.InitialTheta = &theta;
	genParams.carryOverValue = &carryOver;
	genParams.waveParams = &currentWaveParams;
	
	genParams.addNoiseInd = addNoiseIndicator;
	TRACE("Will generate the data of a signal of freq %dl\n", signalFreq);

	size_t i;
	for (i = 0; i < totNumberOfBuffers; i += 1)
	{
		RenderBuffer* renderBuffer = new RenderBuffer();

		if (renderBuffer == NULL)
		{
			TRACE("Unable to allocate render buffer\n");
			return -1;
		}
		renderBuffer->_BufferLength = bufferSizeInBytes;
		renderBuffer->_Buffer = new BYTE[bufferSizeInBytes];

		if (renderBuffer->_Buffer == NULL)
		{
			TRACE("Unable to allocate render buffer\n");
			return -1;
		}

		genParams.Buffer = renderBuffer->_Buffer;
		genParams.BufferLength = renderBuffer->_BufferLength;
		genParams.masterVolume = (float)MasterVolume;

		switch (SampleType)
		{
		case CWASAPIRenderer::SampleTypeFloat:
			
			if (m_signalType == SINE_SIGNAL)
				GenerateSineSamples<float>(&genParams);
			else if (m_signalType == TRIANGLE_SIGNAL)
				GenerateTriangleSamples<float>(&genParams);
			else if (m_signalType == SAW_SIGNAL)
				GenerateSawSamples<float>(&genParams);
			else if (m_signalType == SQUARE_SIGNAL)
				GenerateSquareSamples<float>(&genParams);
			else if (m_signalType == NOISE_SIGNAL)
			{
				if (m_noiseType == WHITE_NOISE_SIGNAL)
				{
					GenerateWhiteNoiseSamples<float>(&genParams);
				}
				else if (m_noiseType == PINK_NOISE_SIGNAL)
				{
					// Modify this when a Brown noise method exists
					//GenerateWhiteNoiseSamples<float>(&genParams);
					GeneratePinkNoiseSamples<float>(&genParams);
				}
				else if (m_noiseType == BROWN_NOISE_SIGNAL)
				{
					// Modify this when a Brown noise method exists
					GenerateWhiteNoiseSamples<float>(&genParams);
				}
			}
			else if (m_signalType == COMPLEX_SIGNAL)
				GenerateMultiSineSamples<float>(&genParams);
			
			else
				GenerateSineSamples<float>(&genParams);


			break;
		case CWASAPIRenderer::SampleType16BitPCM:
			TRACE("CWASAPIRenderer::SampleType16BitPCM\n");

			if (m_signalType == SINE_SIGNAL)
				GenerateSineSamples<short>(&genParams);
			else if (m_signalType == TRIANGLE_SIGNAL)
				GenerateTriangleSamples<short>(&genParams);
			else if (m_signalType == SAW_SIGNAL)
				GenerateSawSamples<short>(&genParams);
			else if (m_signalType == SQUARE_SIGNAL)
				GenerateSquareSamples<short>(&genParams);
			else if (m_signalType == NOISE_SIGNAL)
			{
				if (m_noiseType == WHITE_NOISE_SIGNAL)
				{
					GenerateWhiteNoiseSamples<short>(&genParams);
				}
				else if (m_noiseType == PINK_NOISE_SIGNAL)
				{
					// Modify this when a Brown noise method exists
					//GenerateWhiteNoiseSamples<float>(&genParams);
					GeneratePinkNoiseSamples<short>(&genParams);
				}
				else if (m_noiseType == BROWN_NOISE_SIGNAL)
				{
					// Modify this when a Brown noise method exists
					GenerateWhiteNoiseSamples<float>(&genParams);
				}



			}
				
			else if (m_signalType == COMPLEX_SIGNAL)
				GenerateMultiSineSamples<short>(&genParams);

			break;
		}

		//
		//  Link the newly allocated and filled buffer into the queue.  
		//
		*currentBufferTail = renderBuffer;
		currentBufferTail = &renderBuffer->_Next;
	}

	TRACE("Completed: %d buffers of data of %d bytes each created...\n", i, bufferSizeInBytes);
}


bool CMainDlg::playTone(int freq, int durationInSec)
{

	//params = new PLAY_THREAD_PARAMS();

	//params->frequency = freq;
	//params->durationInSec = durationInSec;

	
	pParamObject = new CThreadParamObject(freq, durationInSec);
	AfxBeginThread(run, this);

	return true;
}


UINT MyThreadProc(LPVOID pParam)
{
	CThreadParamObject* pObject = (CThreadParamObject*)pParam;

	if (pObject == NULL ||
		!pObject->IsKindOf(RUNTIME_CLASS(CThreadParamObject)))
		return 1;   // if pObject is not valid

		// do something with 'pObject'

	return 0;   // thread completed successfully
}




DWORD WINAPI CMainDlg::PlayToneThreadProc(LPVOID lpParam)
{

	TargetDurationInSec = 2;

	TRACE("Play thread invoked...TargetDurationInSec value is %d\n", TargetDurationInSec);
	//PPLAY_THREAD_PARAMS params = (PPLAY_THREAD_PARAMS)lpParam;


	if (pParamObject == NULL)
	{
		TRACE("CMainDlg::PlayToneThreadProc has a null param object!!!\n");
	}

	TargetFrequency = pParamObject->getFrequency();

	//  *** Jan 2020:  See Note #1
	if (renderer->Initialize(TargetLatency))
	{
		generateSignal(TargetFrequency,
			renderer->ChannelCount(),
			renderer->SampleType(),
			renderer->SamplesPerSecond(),
			renderer->FrameSize(),
			renderBufferSizeInBytes,
			renderBufferCount, addNoiseIndicator);

		


		TRACE("Signal generated and will start the CWASAPIRenderer thread.\n");

		//  The renderer takes ownership of the render queue - it will free the items in the queue when it renders them.
		TRACE("CWASAPIRenderer::Render Start...\n");



		if (renderer->Start(renderQueue))
		{
			do
			{
				TRACE(".\n");
				Sleep(1000);
			} while (--TargetDurationInSec);
			TRACE("\n");

			renderer->Stop();
		}
	}


	// *** Make sure to toggle the Button on the MFC screen

	/*Button_Enable(hwndButtonPlay, TRUE);
	Button_Enable(hwndButtonStop, FALSE);*/

	TRACE("Play thread terminated\n\n\n");

	//delete params;
	return 0;
}

// Note: The Sliders in the UI have integer values from 0 to 100
//       while the frequency level are float values from 0.0 to 1.0
//       so a conversion is needed here...
void CMainDlg::updateHarmonicLevels(int harmonicIndex, int value)
{

	// default validation: the index starts at zero for the base harmonic
	// and goes to nbrFrequencies - 1 for the last harmonic
	if (harmonicIndex < 0 || harmonicIndex >= currentWaveParams.nbrFrequencies)  return;
	
	if (value < 0 || value >= 100)  return;

	currentWaveParams.amplitude[harmonicIndex] = (float)value / 100;

	float levelsSum = 0.0f;
	for (int i = 0; i < currentWaveParams.nbrFrequencies; i++)
		levelsSum += currentWaveParams.amplitude[i];

	for (int i = 0; i < currentWaveParams.nbrFrequencies; i++)
		currentWaveParams.amplitude[i] /= levelsSum;
	
}



