// RenderSharedWin32GDI.cpp : Defines the entry point for the application.
//


#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 



#include "framework.h"
#include "RenderSharedWin32GDI.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



HBITMAP radioSineWaveImg = NULL;
HBITMAP radioTriangleWaveImg = NULL;
HBITMAP radioSawWaveImg = NULL;
HBITMAP radioSquareWaveImg = NULL;
HBITMAP radioNoiseWaveImg = NULL;


//bool DisableMMCSS;


//static const GUID		CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E };
//static const GUID		IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 };
//static const GUID		IID_IAudioClient = { 0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2 };
//static const GUID		IID_IAudioRenderClient = { 0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2 };
//static const GUID		PcmSubformatGuid = { STATIC_KSDATAFORMAT_SUBTYPE_PCM };

///////////////////////////////////////////////////////////////////////////////////////////////////
//   signalFreq:         desired frequency of the signal to produce
//   channelCount:       Can be either 1 (mono) or 2 (Stereo)
//   SampleType:        WASAPI related values  --> SampleTypeFloat or SampleType16BitPCM
//   samplesPerSecond:   Number of samples by second to provide to the sound card
//   frameSize:          Size of one full sample:  BitPerSample * channelCount
//   bufferSizeInBytes:  Full size of the byte array to contains all the frames data
//   totNumberOfBuffer:  how many sub buffer required to cover all the time of the signal
int generateSignal(int signalFreq, int channelCount,
    CWASAPIRenderer::RenderSampleType SampleType,
    int samplesPerSecond,
    int frameSize,
    int bufferSizeInBytes, int totNumberOfBuffers)
{
    RenderBuffer** currentBufferTail = &renderQueue;

    double theta     = 0.0;
    double carryOver = 0.0;

    GenParams genParams;


    //genParams.Buffer = renderBuffer->_Buffer;
    //genParams.BufferLength = renderBuffer->_BufferLength;
    genParams.Frequency = TargetFrequency;
    genParams.ChannelCount = channelCount;
    genParams.SamplesPerSecond = samplesPerSecond;
    genParams.InitialTheta = &theta;
    genParams.carryOverValue = &carryOver;



    printf("Will generate the data of a signal of freq %dl\n", signalFreq);

    size_t i;
    for (i = 0; i < totNumberOfBuffers; i += 1)
    {
        RenderBuffer* renderBuffer = new (std::nothrow) RenderBuffer();
        if (renderBuffer == NULL)
        {
            printf("Unable to allocate render buffer\n");
            return -1;
        }
        renderBuffer->_BufferLength = bufferSizeInBytes;
        renderBuffer->_Buffer = new (std::nothrow) BYTE[bufferSizeInBytes];
        if (renderBuffer->_Buffer == NULL)
        {
            printf("Unable to allocate render buffer\n");
            return -1;
        }

        genParams.Buffer = renderBuffer->_Buffer;
        genParams.BufferLength = renderBuffer->_BufferLength;
       

        //theta += 1.0;

        switch (SampleType)
        {
        case CWASAPIRenderer::SampleTypeFloat:

            if (signalType == SINE_SIGNAL)
                //GenerateSineSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                //    channelCount, samplesPerSecond, &theta);
            GenerateSineSamples<float>(&genParams);
            /* else if (signalType == COMPLEX_SINE_SIGNAL)
                 GenerateMultiSineSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                     channelCount, samplesPerSecond, &waveParams);*/
            else if (signalType == TRIANGLE_SIGNAL)
               /* GenerateTriangleSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                    channelCount, samplesPerSecond, &theta);*/
            GenerateTriangleSamples<float>(&genParams);

            else if (signalType == SAW_SIGNAL)
                //GenerateSawSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                //    channelCount, samplesPerSecond, &theta);
                GenerateSawSamples<float>(&genParams);
            else if (signalType == WHITE_NOISE_SIGNAL)
                //GenerateWhiteNoiseSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                //    channelCount, samplesPerSecond);
                GenerateWhiteNoiseSamples<float>(&genParams);

            else
              /*  GenerateSineSamples<float>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                    channelCount, samplesPerSecond, &theta);*/
                GenerateSineSamples<float>(&genParams);


            break;
        case CWASAPIRenderer::SampleType16BitPCM:
            printf("CWASAPIRenderer::SampleType16BitPCM\n");

            if (signalType == SINE_SIGNAL)
                    //GenerateSineSamples<short>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                    // channelCount, samplesPerSecond, &theta);
                    GenerateSineSamples<short>(&genParams);
            
            else if (signalType == TRIANGLE_SIGNAL)
                //GenerateTriangleSamples<short>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                //    channelCount, samplesPerSecond, &theta);
                GenerateTriangleSamples<short>(&genParams);
            else if (signalType == SAW_SIGNAL)
                //GenerateSawSamples<short>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                //    channelCount, samplesPerSecond, &theta);
                GenerateSawSamples<short>(&genParams);
            else if (signalType == WHITE_NOISE_SIGNAL)
    /*            GenerateWhiteNoiseSamples<short>(renderBuffer->_Buffer, renderBuffer->_BufferLength, TargetFrequency,
                       channelCount, samplesPerSecond);
    */            GenerateWhiteNoiseSamples<short>(&genParams);

            break;
        }

        //
        //  Link the newly allocated and filled buffer into the queue.  
        //
        *currentBufferTail = renderBuffer;
        currentBufferTail = &renderBuffer->_Next;
    }

    printf("Completed: %d buffers of data of %d bytes each created...\n", i, bufferSizeInBytes);
}

//
//  Retrieves the device friendly name for a particular device in a device collection.
//
LPWSTR GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex)
{
    IMMDevice* device;
    LPWSTR deviceId;
    HRESULT hr;

    hr = DeviceCollection->Item(DeviceIndex, &device);
    if (FAILED(hr))
    {
        printf("Unable to get device %d: %x\n", DeviceIndex, hr);
        return NULL;
    }
    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
        return NULL;
    }

    IPropertyStore* propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    SafeRelease(&device);
    if (FAILED(hr))
    {
        printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
        return NULL;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    SafeRelease(&propertyStore);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    wchar_t deviceName[128];
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
    if (FAILED(hr))
    {
        printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    wchar_t* returnValue = _wcsdup(deviceName);
    if (returnValue == NULL)
    {
        printf("Unable to allocate buffer for return\n");
        return NULL;
    }
    return returnValue;
}

//
//  Retrieves the device friendly name for a particular device in a device collection.
//
LPWSTR GetDeviceCompleteName(IMMDevice* device)
{
    LPWSTR deviceId;
    HRESULT hr;



    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device id: \n");
        return NULL;
    }


    IPropertyStore* propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    //SafeRelease(&device);
    if (FAILED(hr))
    {
        printf("Unable to open device property store: %x\n", hr);
        return NULL;
    }



    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    SafeRelease(&propertyStore);

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device %d : %x\n", hr);
        return NULL;
    }



    wchar_t deviceName[128];
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
    if (FAILED(hr))
    {
        printf("Unable to format friendly name for device %d : %x\n", hr);
        return NULL;
    }

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    wchar_t* returnValue = _wcsdup(deviceName);
    if (returnValue == NULL)
    {
        printf("Unable to allocate buffer for return\n");
        return NULL;
    }


    return returnValue;
}


LPWSTR  getMixFormatWaveString(WAVEFORMATEX* wf)
{
    printf("Format type:  %d\n", wf->wFormatTag);
    printf("Number of channels (i.e. mono, stereo...):  %d\n", wf->nChannels);
    printf("Sample rate:   %d\n", wf->nSamplesPerSec);
    printf("for buffer estimation:  %d\n", wf->nAvgBytesPerSec);
    printf("Block size of data:  %d\n", wf->nBlockAlign);
    printf("Number of bits per sample of mono data:  %d\n", wf->wBitsPerSample);
    printf("bytes count of  extra information (after cbSize): %d\n", wf->cbSize);

    return NULL;
}


//
//  Based on the input switches, pick the specified device to use.
//
bool PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole)
{
    HRESULT hr;
    bool retValue = true;
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDeviceCollection* deviceCollection = NULL;

    *IsDefaultDevice = false;   // Assume we're not using the default device.

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        printf("Unable to instantiate device enumerator: %x\n", hr);
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
            printf("Unable to retrieve device collection: %x\n", hr);
            retValue = false;
            goto Exit;
        }

        printf("Available an output device(s):\n");
        printf("    0:  Default Console Device\n");
        printf("    1:  Default Communications Device\n");
        printf("    2:  Default Multimedia Device\n");
        UINT deviceCount;
        hr = deviceCollection->GetCount(&deviceCount);
        if (FAILED(hr))
        {
            printf("Unable to get device collection length: %x\n", hr);
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
            printf("    %d:  %S\n", i + 3, deviceName);
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
            printf("unrecognized device index: %S\n", choice);
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
                printf("Unable to retrieve device %d: %x\n", deviceIndex - 3, hr);
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
            printf("Unable to get endpoint for endpoint %S: %x\n", OutputEndpoint, hr);
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
            printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
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




void connectControlsToHandle(HWND hWnd)
{
    hwndButtonPlay = GetDlgItem(hWnd, IDM_BUTTON_PLAY);
    hwndButtonStop = GetDlgItem(hWnd, IDM_BUTTON_STOP);
    Button_Enable(hwndButtonStop, FALSE);

    hFrequencyTrack = GetDlgItem(hWnd, IDS_FREQVALUE_TRACKER);
    hFrequencyValueLabel = GetDlgItem(hWnd, IDS_FREQVALUE_TRACKER_LABEL);
    hFrequencyLeftLabel = GetDlgItem(hWnd, IDS_LEFT_VOLUME_TRACKER_LABEL);
    hFrequencyRightLabel = GetDlgItem(hWnd, IDS_RIGHT_VOLUME_TRACKER_LABEL);

    
    SendMessageW(hFrequencyTrack, TBM_SETRANGE, TRUE, MAKELONG(MINIMAL_FREQUENCY, MAXMIMAL_FREQUENCY));
    SendMessageW(hFrequencyTrack, TBM_SETPAGESIZE, 0, 10);
    SendMessageW(hFrequencyTrack, TBM_SETTICFREQ, 28, 0);
    SendMessageW(hFrequencyTrack, TBM_SETPOS, FALSE, 0);
    /*SendMessageW(hFrequencyTrack, TBM_SETBUDDY, TRUE, (LPARAM)hFrequencyLeftLabel);
    SendMessageW(hFrequencyTrack, TBM_SETBUDDY, FALSE, (LPARAM)hFrequencyRightLabel);*/




    hVolumeTrack = GetDlgItem(hWnd, IDS_VOLUME_VALUE_TRACKER);
    hVolumeValueLabel = GetDlgItem(hWnd, IDS_VOLUME_VALUE_TRACKER_LABEL);

    SendMessageW(hVolumeTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    SendMessageW(hVolumeTrack, TBM_SETPAGESIZE, 0, 10);
    SendMessageW(hVolumeTrack, TBM_SETTICFREQ, 1, 0);
    SendMessageW(hVolumeTrack, TBM_SETPOS, FALSE, 0);

    hSineWaveRadioButton     = GetDlgItem(hWnd, IDC_SINE_SIGNAL);
    hSawWaveRadioButton      = GetDlgItem(hWnd, IDC_SAW_SIGNAL);
    hTriangleWaveRadioButton = GetDlgItem(hWnd, IDC_TRIANGLE_SIGNAL);
    hSquareWaveRadioButton = GetDlgItem(hWnd, IDC_SQUARE_SIGNAL);
    hNoiseWaveRadioButton    = GetDlgItem(hWnd, IDC_NOISE_SIGNAL);


    radioSineWaveImg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SINE_WAVE));
    radioTriangleWaveImg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_TRIANGLE_WAVE));
    radioSawWaveImg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SAW_WAVE));
    radioSquareWaveImg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SQUARE_WAVE));
    radioNoiseWaveImg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_NOISE_WAVE));

    SendMessage(hSineWaveRadioButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)radioSineWaveImg);
    SendMessage(hTriangleWaveRadioButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)radioTriangleWaveImg);
    SendMessage(hSawWaveRadioButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)radioSawWaveImg);
    SendMessage(hSquareWaveRadioButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)radioSquareWaveImg);
    SendMessage(hNoiseWaveRadioButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)radioNoiseWaveImg);



}





//void  buildFrequencyTracker(HWND hWnd)
//{
//    hFrequencyTrack = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
//                                    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
//                                    45, 50, 170, 30, hWnd, (HMENU)IDS_FREQVALUE_TRACKER, NULL, NULL);
//
//    // Build the Slider Control for setting the Frequency of the signal
//    hFrequencyLeftLabel = CreateWindow(L"Static", L"220",
//        WS_CHILD | WS_VISIBLE,
//        10, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_LEFT_FREQ_TRACKER_LABEL, NULL, NULL);
//
//    hFrequencyRightLabel = CreateWindow(L"Static", L"880",
//        WS_CHILD | WS_VISIBLE,
//        220, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_RIGHT_FREQ_TRACKER_LABEL, NULL, NULL);
//
//    hFrequencyValueLabel = CreateWindow(L"Static", L"220", WS_CHILD | WS_VISIBLE,
//        100, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_FREQVALUE_TRACKER_LABEL, NULL, NULL);
//
//
//    SendMessageW(hFrequencyTrack, TBM_SETRANGE, TRUE, MAKELONG(MINIMAL_FREQUENCY, MAXMIMAL_FREQUENCY));
//    SendMessageW(hFrequencyTrack, TBM_SETPAGESIZE, 0, 10);
//    SendMessageW(hFrequencyTrack, TBM_SETTICFREQ, 28, 0);
//    SendMessageW(hFrequencyTrack, TBM_SETPOS, FALSE, 0);
//    SendMessageW(hFrequencyTrack, TBM_SETBUDDY, TRUE, (LPARAM)hFrequencyLeftLabel);
//    SendMessageW(hFrequencyTrack, TBM_SETBUDDY, FALSE, (LPARAM)hFrequencyRightLabel);
//
//}

//
//void  buildVolumeTracker(HWND hWnd)
//{
//
//    hVolumeTrack = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
//        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
//        345, 50, 170, 30, hWnd, (HMENU)IDS_VOLUME_VALUE_TRACKER, NULL, NULL);
//
//
//    // Build the Slider Control for setting the Frequency of the signal
//    hVolumeLeftLabel = CreateWindow(L"Static", L"0",
//        WS_CHILD | WS_VISIBLE,
//        310, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_LEFT_FREQ_TRACKER_LABEL, NULL, NULL);
//
//    hVolumeRightLabel = CreateWindow(L"Static", L"10",
//        WS_CHILD | WS_VISIBLE,
//        520, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_RIGHT_FREQ_TRACKER_LABEL, NULL, NULL);
//
//    hVolumeValueLabel = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE,
//        400, 85,
//        DEFAULT_LABEL_WIDTH, DEFAULT_LABEL_HEIGHT,
//        hWnd, (HMENU)IDS_FREQVALUE_TRACKER_LABEL, NULL, NULL);
//
//    SendMessageW(hVolumeValueLabel, TBM_SETRANGE, TRUE, MAKELONG(MINIMAL_VOLUME, MAXMIMAL_VOLUME));
//    SendMessageW(hVolumeValueLabel, TBM_SETPAGESIZE, 0, 1);
//    SendMessageW(hVolumeValueLabel, TBM_SETTICFREQ, 1, 0);
//    SendMessageW(hVolumeValueLabel, TBM_SETPOS, FALSE, 50);
//    SendMessageW(hVolumeValueLabel, TBM_SETBUDDY, TRUE, (LPARAM)hVolumeLeftLabel);
//    SendMessageW(hVolumeValueLabel, TBM_SETBUDDY, FALSE, (LPARAM)hVolumeRightLabel);
//
//}







bool initWASAPI()
{
    printf("Initialisation of WASAPI...\n");
    //
  //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
  //
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        printf("Unable to initialize COM: %x\n", hr);
        result = hr;
        return false;
    }
    printf("CoInitializeEx using CoInitializeEx\n");

    //
    //  Now that we've parsed our command line, pick the device to render.
    //
    if (!PickDevice(&device, &isDefaultDevice, &role))
    {
        result = -1;
        return false;
    }
    printf("Device picked successfully\n");


    renderer = new (std::nothrow) CWASAPIRenderer(device, isDefaultDevice, role);
    if (renderer == NULL)
    {
        printf("Unable to allocate renderer\n");
        false;
    }
    printf("CWASAPIRenderer instance created\n");



    if (renderer->Initialize(TargetLatency))
    {
        renderBufferSizeInBytes = (renderer->BufferSizePerPeriod() * renderer->FrameSize());
        renderDataLength = (renderer->SamplesPerSecond() * TargetDurationInSec * renderer->FrameSize()) + (renderBufferSizeInBytes - 1);
        renderBufferCount = renderDataLength / (renderBufferSizeInBytes);

        printf("CWASAPIRenderer initiated with success\n");
        printf("Buffer Size per Period  = %d\n", renderer->BufferSizePerPeriod());
        printf("Frame Size = %d\n", renderer->FrameSize());

        printf("Buffer Size In Bytes = %d\n", renderBufferSizeInBytes);
        printf("Data Length = %d\n", renderBufferSizeInBytes);
        printf("Total number of Buffers = %d\n", renderBufferCount);
    }
    else 
    {
        printf("CWASAPIRenderer initialisation failed!\n");
        return false;
    }
        
    printf("WASAPI Initialisation is completed\n");

    return true;
}

bool terminateWASAPI()
{
    printf("Will Terminate of WASAPI...\n");
   
    if (renderer != NULL)
    {
        renderer->Shutdown();
        SafeRelease(&renderer);
        printf("Released the CWASAPIRenderer class\n");
    }
   
    SafeRelease(&device);
    CoUninitialize();
    printf("WASAPI is realeased and terminated\n");
    return true;
}


bool playTone(int freq, int durationInSec)
{

    params = new PLAY_THREAD_PARAMS();
    
    params->frequency = freq;
    params->durationInSec = durationInSec;


    Button_Enable(hwndButtonPlay, FALSE);
    Button_Enable(hwndButtonStop, TRUE);


    // Create a thread
    hPlayThread = CreateThread(  NULL,         // default security attributes
                                0,            // default stack size
                                (LPTHREAD_START_ROUTINE)PlayToneThreadProc,
                                params,         // no thread function arguments
                                0,            // default creation flags
                                &dwTPlayhreadID); // receive thread identifier

    if (hPlayThread == NULL)
    {
        printf("CreateThread error: %d\n", GetLastError());
        return false;
    }

    return true;
}


DWORD WINAPI PlayToneThreadProc(LPVOID lpParam)
{

    TargetDurationInSec = 2;

    printf("Play thread invoked...TargetDurationInSec value is %d\n", TargetDurationInSec);
    PPLAY_THREAD_PARAMS params = (PPLAY_THREAD_PARAMS)lpParam;

    //  *** Jan 2020:  See Note #1
    if (renderer->Initialize(TargetLatency))
    {
        generateSignal(TargetFrequency,
            renderer->ChannelCount(),
            renderer->SampleType(),
            renderer->SamplesPerSecond(),
            renderer->FrameSize(),
            renderBufferSizeInBytes,
            renderBufferCount);
        printf("Signal generated and will start the CWASAPIRenderer thread.\n");

        //  The renderer takes ownership of the render queue - it will free the items in the queue when it renders them.
        printf("CWASAPIRenderer::Render Start...\n");

       

        if (renderer->Start(renderQueue))
        {
            do
            {
                printf(".");
                Sleep(1000);
            } while (--TargetDurationInSec);
            printf("\n");

            renderer->Stop();
        }
    }
   

    Button_Enable(hwndButtonPlay, TRUE);
    Button_Enable(hwndButtonStop, FALSE );

    printf("Play thread terminated\n\n\n");

    delete params;
    return 0;
}


void testSignals()
{
    int signalFreq = 440;
    int channelCount = 2;
    int samplesPerSecond = 48000;
    int frameSize = 8;
    int bufferSizeInBytes = 4800;
    int totNumberOfBuffers = 160;

    int bufferIndex = 0;

    double theta = 0.0;


    std::ofstream MyFile("triangleData.txt");



    // signalType = SINE_SIGNAL;
 //    signalType = COMPLEX_SINE_SIGNAL  ;
    signalType = TRIANGLE_SIGNAL;
    //    signalType = SAW_SIGNAL           ;
     //   signalType = WHITE_NOISE_SIGNAL   ;

    generateSignal(signalFreq, channelCount,
        CWASAPIRenderer::RenderSampleType::SampleTypeFloat,
        samplesPerSecond, frameSize, bufferSizeInBytes, totNumberOfBuffers);


    bool firstPass = true;
    while (renderQueue != NULL)
    {
        RenderBuffer* renderBuffer = renderQueue;
        renderQueue = renderBuffer->_Next;



        if (firstPass)
        {
            float* dataBuffer = reinterpret_cast<float*>(renderBuffer->_Buffer);

            for (size_t i = 0; i < bufferSizeInBytes / sizeof(float); i += channelCount)
            {
                MyFile << dataBuffer[i] << "\n";
            }
        }

        delete renderBuffer;
    }

    // Close the file
    MyFile.close();
}



void  createUserForm(HWND hWnd)
{

    INITCOMMONCONTROLSEX icex;

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);



   


    connectControlsToHandle(hWnd);

    /*  buildButtons(hWnd);
      buildRadioSection(hWnd);
      buildFrequencyTracker(hWnd);
      buildVolumeTracker(hWnd);*/
}

void UpdateFrequencyLabel(void) {

    TargetFrequency = 1;

    LRESULT pos = SendMessageW(hFrequencyTrack, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    wsprintfW(buf, L"%ld", pos);

    SetWindowTextW(hFrequencyValueLabel, buf);

    TargetFrequency = LOWORD(pos);
}

void UpdateVolumeLabel(void) {

    
    LRESULT pos = SendMessageW(hVolumeTrack, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    wsprintfW(buf, L"%ld", pos);

    SetWindowTextW(hVolumeValueLabel, buf);

    MasterVolume = LOWORD(pos);

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RENDERSHAREDWIN32GDI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RENDERSHAREDWIN32GDI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable



    HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)WndProc);




    /* HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                               SCREEN_X_POS, SCREEN_Y_POS,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               nullptr, nullptr, hInstance, nullptr);*/

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


        case  WM_INITDIALOG:
        {
            createUserForm(hWnd);
            initWASAPI();
        }
        break;


        case WM_VSCROLL:
        {
            int wmId = LOWORD(wParam);
            UpdateFrequencyLabel();
        }
        break;

    
        case WM_HSCROLL:
            {
                int wmId = LOWORD(wParam);
                UpdateVolumeLabel();
            }
            break;

        case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                // Parse the menu selections:
                switch (wmId)
                {
                case IDM_BUTTON_PLAY:
                    playTone(TargetFrequency, TargetDurationInSec);              
                    break;
                case IDM_BUTTON_STOP:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;

                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;

                //case IDM_EXIT:
                case IDB_CLOSE_APP:
                    DestroyWindow(hWnd);
                    break;

                case IDC_SINE_SIGNAL:
                    signalType = SINE_SIGNAL;
                    break;

                case IDC_TRIANGLE_SIGNAL:
                    signalType = TRIANGLE_SIGNAL;
                    break;

                case IDC_SQUARE_SIGNAL:
                     MessageBox(hWnd, L"Error", L"Not Implemented!", MB_ICONERROR | MB_OK);
                    signalType = SAW_SIGNAL;
                    break;

                case IDC_SAW_SIGNAL:
                    signalType = SAW_SIGNAL;
                    break;



                case IDC_NOISE_SIGNAL: 
                    signalType = WHITE_NOISE_SIGNAL;
                    break;


            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        terminateWASAPI();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RENDERSHAREDWIN32GDI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RENDERSHAREDWIN32GDI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



int main() {

    // Calling the wWinMain function to start the GUI program
    // Parameters:
    // GetModuleHandle(NULL) - To get a handle to the current instance
    // NULL - Previous instance is not needed
    // NULL - Command line parameters are not needed
    // 1 - To show the window normally
    wWinMain(GetModuleHandle(NULL), NULL, NULL, 1);

    system("pause");
    return 0;
}





//////////////////////////////////////////////////////////////////////////////////////////
/// Notes Section:
  
// Note #1 Jan 2020:   
// Tried to pre initialize the Renderer during the application init,
//  and it is not working. So, the Render appears to have to be reinitialized for
//  each tone we are going to play...  Not sure why...
