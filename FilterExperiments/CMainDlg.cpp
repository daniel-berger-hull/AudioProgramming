
// CMainDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FilterExperimentApp.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include "ToneGen.h"

//#include "WavLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainDlg dialog


typedef struct PlayThreadParams {
	int frequency;
	int durationInSec;
} PLAY_THREAD_PARAMS, * PPLAY_THREAD_PARAMS;



//WaveParams  currentWaveParams;

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTER_EXPERIMENTS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMainDlg::~CMainDlg()
{
	if (wavLoader != NULL)     delete wavLoader;
}



void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Control(pDX, IDC_MASTER_VOLUME_SLIDER, m_masterVolumeSlider);
	DDX_Control(pDX, IDC_PLAY_BUTTON, m_playButton);
	DDX_Control(pDX, IDC_STOP_BUTTON, m_stopButton);
	DDX_Control(pDX, IDC_LOAD_WAV_FILE, m_loadWavFileButton);
	DDX_Control(pDX, IDC_CLEAR_WAV_FILE_BUTTON_VALUE, m_clearWavFileButton);
	DDX_Control(pDX, IDC_WAV_FILE_EDIT, m_wavFileNameEdit);

	DDX_Text(pDX, IDC_MASTER_VOLUME_LABEL, m_masterVolumeValue);
	
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()

	ON_BN_CLICKED(IDC_PLAY_BUTTON, &CMainDlg::OnBnClickedPlayButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CMainDlg::OnBnClickedStopButton)
	ON_BN_CLICKED(IDC_LOAD_WAV_FILE, &CMainDlg::OnBnClickedLoadWavFile)
	ON_BN_CLICKED(IDC_CLEAR_WAV_FILE_BUTTON_VALUE, &CMainDlg::OnBnClickedClearWavFileButtonValue)
	ON_EN_CHANGE(IDC_WAV_FILE_EDIT, &CMainDlg::OnEnChangeWavFileEdit)
	ON_BN_CLICKED(IDC_LOW_PASS_RADIO, &CMainDlg::OnBnClickedLowPassRadio)
	ON_BN_CLICKED(IDC_HIGH_PASS_RADIO, &CMainDlg::OnBnClickedHighPassRadio)

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

	m_masterVolumeSlider.SetRange(0, 100);
	m_masterVolumeSlider.SetPos(100);
	m_masterVolumeValue.Format(_T("%d"), 100);
}



//void CMainDlg::decorateRadioButtonSection()
//{
//	HBITMAP sineBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SINE_WAVE));
//
//	CWnd* sineRadioButton = GetDlgItem(IDC_SINE_WAVE_RADIO);
//
//	sineRadioButton->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)sineBitmap);
//}



BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	m_backgroundBrush.CreateSolidBrush(RGB(0, 84, 165));

	setupSliders();
	
	UpdateData(FALSE);


	//TEST_WAVE_FILENAME


	//m_wavFileNameEdit.GetWindowTextW();

	m_wavFileNameEdit.SetWindowTextW(_T(TEST_WAVE_FILENAME));


	return TRUE;  // return TRUE  unless you set the focus to a control
}




HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);



	if (nCtlColor == CTLCOLOR_STATIC )
	{
		
		if (pWnd->GetDlgCtrlID() == IDC_MASTER_VOLUME_SLIDER  )
		{
			//pDC->SetTextColor(RGB(255, 255, 255));

			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}

		if ( pWnd->GetDlgCtrlID() == IDC_MASTER_VOLUME_LABEL )
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush; 
		}

		if (pWnd->GetDlgCtrlID() == IDC_LOW_PASS_RADIO ||
			pWnd->GetDlgCtrlID() == IDC_HIGH_PASS_RADIO)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_backgroundBrush;
		}


		if (pWnd->GetDlgCtrlID() == IDC_GROUP_VOLUME ||
			pWnd->GetDlgCtrlID() == IDC_WAV_FILE_GROUP ||
			pWnd->GetDlgCtrlID() == IDC_FILTERS_GROUP  )
		{
			pDC->SetTextColor(RGB(255,255,255));
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
	playTone();
	m_playButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
}


void CMainDlg::OnBnClickedStopButton()
{
	//There is nothing done here, but it should be able to stop
	//the rendering of the audio file when clicking on the stop button
	m_playButton.EnableWindow(TRUE);
	m_stopButton.EnableWindow(FALSE);
}


void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == (CScrollBar*)&m_masterVolumeSlider)
	{
		int value = m_masterVolumeSlider.GetPos();
		MasterVolume = value;

		m_masterVolumeValue.Format(_T("%d"), value);
		UpdateData(FALSE);
	}
	else {
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}



int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	initWASAPI();
	return 0;
}


void CMainDlg::OnDestroy()
{
	terminateWASAPI();
	CDialogEx::OnDestroy();
}





void CMainDlg::OnBnClickedLoadWavFile()
{
	CString test = m_wavFileNameValue;

	CString sWindowText;
	m_wavFileNameEdit.GetWindowText(sWindowText);

	AfxBeginThread(runFileLoader, this);
}


void CMainDlg::OnBnClickedClearWavFileButtonValue()
{
	m_wavFileNameEdit.SetWindowTextW(_T(""));

	m_loadWavFileButton.EnableWindow(FALSE);
	m_clearWavFileButton.EnableWindow(FALSE);
}


void CMainDlg::OnBnClickedLowPassRadio()
{
	int a = 2;
	// TODO: Add your control notification handler code here
}


void CMainDlg::OnBnClickedHighPassRadio()
{
	int a = 2;
	// TODO: Add your control notification handler code here
}


void CMainDlg::OnEnChangeWavFileEdit()
{
	m_loadWavFileButton.EnableWindow(TRUE);
	m_clearWavFileButton.EnableWindow(TRUE);
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
		TRACE("A) Buffer Size per Period  = %d\n", renderer->BufferSizePerPeriod());
		TRACE("B) Frame Size = %d\n", renderer->FrameSize());
		TRACE("C) Sample by Seconds = %d\n", renderer->SamplesPerSecond());
		TRACE("\nD) Buffer Size In Bytes = %d  (A * B)\n", renderBufferSizeInBytes);

		TRACE("E) Data Length = %d (C * DurationInSec * B)\n", renderDataLength);
		TRACE("F) Total count of Buffers = %d (E / D)\n", renderBufferCount);
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
//   channelCount:       Can be either 1 (mono) or 2 (Stereo)
//   SampleType:        WASAPI related values  --> SampleTypeFloat or SampleType16BitPCM
//   samplesPerSecond:   Number of samples by second to provide to the sound card
//   frameSize:          Size of one full sample:  BitPerSample * channelCount
//   bufferSizeInBytes:  Full size of the byte array to contains all the frames data
//   totNumberOfBuffer:  how many sub buffer required to cover all the time of the signal

int CMainDlg::loadSignal(int channelCount,
						CWASAPIRenderer::RenderSampleType SampleType,
						int samplesPerSecond,
						int frameSize,
						int bufferSizeInBytes, int totNumberOfBuffers)
{
	RenderBuffer** currentBufferTail = &renderQueue;

	GenParams genParams;

	genParams.ChannelCount = channelCount;
	genParams.SamplesPerSecond = samplesPerSecond;


	size_t i;
	int currentBufferIndex = 0;
	int16_t* leftBufferPtr = wavLoader->getLeftChannelBuffer();
	int16_t* rightBufferPtr = wavLoader->getRightChannelBuffer();
	int16_t  samplesByBuffers = bufferSizeInBytes / frameSize;

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
			LoadWavSamples<float>(&genParams, leftBufferPtr, rightBufferPtr, LOW_PASS_FILTER);
			break;

		case CWASAPIRenderer::SampleType16BitPCM:
			LoadWavSamples<short>(&genParams, leftBufferPtr, rightBufferPtr, LOW_PASS_FILTER);

			break;
		}

		leftBufferPtr += samplesByBuffers;
		rightBufferPtr += samplesByBuffers;

		//
		//  Link the newly allocated and filled buffer into the queue.  
		//
		*currentBufferTail = renderBuffer;
		currentBufferTail = &renderBuffer->_Next;
	}

	TRACE("Completed: %d buffers of data of %d bytes each created...\n", i, bufferSizeInBytes);
}



bool CMainDlg::playTone()
{	
	AfxBeginThread(run, this);

	return true;
}




UINT CMainDlg::runFileLoader(LPVOID pParam)
{

	TRACE(" CListSizeDraw::run(LPVOID p) static version...\n");
	CMainDlg* me = (CMainDlg*)pParam;
	me->runFileLoaderThread();

	return 0;   // thread completed successfully
}

void CMainDlg::runFileLoaderThread()
{

	TRACE(" CMainDlg::runFileLoaderThread() Will load the file..\n");

	CString sWindowText;
	m_wavFileNameEdit.GetWindowText(sWindowText);

	if (sWindowText.IsEmpty())  return;

	
	if (wavLoader == NULL )  wavLoader = new WavLoader();
	wavLoader->open(TEST_WAVE_FILENAME);


	m_playButton.EnableWindow(TRUE);

}


DWORD WINAPI CMainDlg::PlayToneThreadProc(LPVOID lpParam)
{

	TargetDurationInSec = 4;

	TRACE("Play thread invoked...TargetDurationInSec value is %d\n", TargetDurationInSec);
	//PPLAY_THREAD_PARAMS params = (PPLAY_THREAD_PARAMS)lpParam;


	
	//  *** Jan 2022:  See Note #1
	if (renderer->Initialize(TargetLatency))
	{
		loadSignal(
			renderer->ChannelCount(),
			renderer->SampleType(),
			renderer->SamplesPerSecond(),
			renderer->FrameSize(),
			renderBufferSizeInBytes,
			renderBufferCount);

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

	m_playButton.EnableWindow(TRUE);
	m_stopButton.EnableWindow(FALSE);

	//delete params;
	return 0;
}









