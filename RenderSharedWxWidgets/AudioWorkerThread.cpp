//#include "wx/thread.h"

#include <wx/wx.h>

#include "EventID.h"


#include "MainFrame.h"
#include "AudioWorkerThread.h"

#include <functiondiscoverykeys.h>
#include "ToneGen.h"
#include "Tone.h"




WaveParams  currentWaveParams;


AudioWorkerThread::AudioWorkerThread(MainFrame* frame)
    : wxThread()
{
    m_frame = frame;
   // m_count = 0;

    nextCommand = NO_COMMAND;

    logFile.open("AudioThread.log");
    logFile << "Audio Thread Started..." << "\n";
}





wxThread::ExitCode AudioWorkerThread::Entry()
{


	UseConsoleDevice = false;
	UseCommunicationsDevice = false;
	UseMultimediaDevice = false;
	OutputEndpoint = NULL;

#if TEST_YIELD_RACE_CONDITION
    if (TestDestroy())
        return NULL;

    wxThreadEvent event(wxEVT_THREAD, WORKER_EVENT);

    event.SetInt(50);
    wxQueueEvent(m_frame, event.Clone());

    event.SetInt(-1);
    wxQueueEvent(m_frame, event.Clone());
#else
    for (int i = 0; i < 10000; i++)
    {
        // check if we were asked to exit
        if (TestDestroy())
            break;


        int a = 2;

//        if (!commands.empty() )
        if (nextCommand != NO_COMMAND)
        {

         
            //int nextCommand = commands.front();
            //commands.pop_front();

            switch (nextCommand)
            {
                case INIT_WASAPI_COMMAND:
                    log("Audio Thread: INIT_WASAPI_COMMAND received...");
					initWASAPI();
                    break;

                case PLAY_COMMAND:
                    log("Audio Thread: PLAY_COMMAND received...");

					if (commands.size() == 4)
					{
						auto it = commands.begin();
						MasterVolume = *it++;
						TargetFrequency = *it++;
						TargetDurationInSec = *it++;
						signalType = *it;

						PlayToneThreadProc(TargetDurationInSec, TargetFrequency, signalType);					
					}

					
					
                    break;

                case TERMINATE_AUDIO_THREAD_COMMAND:
                    log("Audio Thread: TERMINATE_AUDIO_THREAD_COMMAND received...");
					terminateWASAPI();
                    return NULL;
                    break;
            }

            nextCommand = NO_COMMAND;
        }

     
        wxMilliSleep(50);
    }


 
#endif

    return NULL;
}

void AudioWorkerThread::OnExit()
{
	log("Audio Thread: Thread is existing...");
    logFile.close();
    postEvent(ID_AUDIO_WORKER_EVENT_COMPLETE);
}




void AudioWorkerThread::PlayToneThreadProc(int TargetDurationInSec, int TargetFrequency, int signalType)
{
	
	log("Play thread invoked --> TargetDurationInSec value is ", TargetDurationInSec);
	
	m_audioThreadTickCount = 0;
	//  *** Jan 2020:  See Note #1
	if (renderer->Initialize(TargetLatency))
	{
		generateSignal(TargetFrequency,
			renderer->ChannelCount(),
			renderer->SampleType(),
			renderer->SamplesPerSecond(),
			renderer->FrameSize(),
			renderBufferSizeInBytes,
			renderBufferCount, 
			signalType,
			addNoiseIndicator);


		log("Signal generated and will start the CWASAPIRenderer thread.");

		//  The renderer takes ownership of the render queue - it will free the items in the queue when it renders them.
		log("CWASAPIRenderer::Render Start...");



		if (renderer->Start(renderQueue))
		{ 
			do
			{
				postEvent(ID_AUDIO_WORKER_PLAY_IN_PROGRESS_EVENT_UPDATE);
				Sleep(1000);
				m_audioThreadTickCount++;
			} while (--TargetDurationInSec);
			
			renderer->Stop();
			postEvent(ID_AUDIO_WORKER_PLAY_COMPLETED_EVENT);
			
		}
	}


	// *** Make sure to toggle the Button on the MFC screen

	/*Button_Enable(hwndButtonPlay, TRUE);
	Button_Enable(hwndButtonStop, FALSE);*/

	log("Play thread terminated\n");

	return;

}


bool AudioWorkerThread::postCommand(int commandID)
{
    // Using a list to cache the commands from the AudioWorkerThread.cpp
    // would be a much better design!
    // But The list is crashing, after the second command is posted-Processed and not sure why!  
    //commands.push_back(commandID);

    nextCommand = commandID;

    return true;
}


bool AudioWorkerThread::postCommand(int commandID, std::list<int> params)
{
	// Using a list to cache the commands from the AudioWorkerThread.cpp
   // would be a much better design!
   // But The list is crashing, after the second command is posted-Processed and not sure why!  
   //commands.push_back(commandID);

	commands.clear();

	if (!params.empty())
	{
		for (auto it = params.begin(); it != params.end(); ++it)
			commands.push_back(*it);
	}

	nextCommand = commandID;

	return true;
}



// This method will send an event to the Frame owner
// This can be used to update the display of the main application
// about the current status of the Audio Thread...
void AudioWorkerThread::postEvent(int eventID)
{
    wxThreadEvent event(wxEVT_THREAD, eventID);
    event.SetInt(m_audioThreadTickCount);
    wxQueueEvent(m_frame, event.Clone());
}


void AudioWorkerThread::log(wxString message)
{
	logFile << message << "\n";
}


void AudioWorkerThread::log(wxString message, int value)
{
    wxString mystring = wxString::Format(wxT("%i"), value);
    message.append(mystring);
	logFile << message << "\n";
}

////////////////////////////////////////////////////////////////////  
// Those functions below may be transfered to a WASAPI sound class
////////////////////////////////////////////////////////////////////  


bool AudioWorkerThread::initWASAPI()
{
	//log("Initialisation of WASAPI...\n");
	//
  //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
  //
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		log("Unable to initialize COM: ", hr);
		result = hr;
		return false;
	}
	log("CoInitializeEx using CoInitializeEx");

	//
	//  Now that we've parsed our command line, pick the device to render.
	//
	if (!PickDevice(&device, &isDefaultDevice, &role))
	{
		result = -1;
		return false;
	}
	log("Device picked successfully");

	renderer = new CWASAPIRenderer(device, isDefaultDevice, role);


	if (renderer == NULL)
	{
		log("Unable to allocate renderer");
		false;
	}
	log("CWASAPIRenderer instance created");



	if (renderer->Initialize(TargetLatency))
	{
		renderBufferSizeInBytes = (renderer->BufferSizePerPeriod() * renderer->FrameSize());
		renderDataLength = (renderer->SamplesPerSecond() * TargetDurationInSec * renderer->FrameSize()) + (renderBufferSizeInBytes - 1);
		renderBufferCount = renderDataLength / (renderBufferSizeInBytes);

		log("CWASAPIRenderer initiated with success");
		log("Buffer Size per Period  = ", renderer->BufferSizePerPeriod());
		log("Frame Size = ", renderer->FrameSize());

		log("Buffer Size In Bytes = ", renderBufferSizeInBytes);
		log("Data Length = ", renderBufferSizeInBytes);
		log("Total number of Buffers = ", renderBufferCount);
	}
	else
	{
		log("CWASAPIRenderer initialisation failed!");
		return false;
	}

	log("WASAPI Initialisation is completed");
	wasapInit = true;

	return true;
}

bool AudioWorkerThread::terminateWASAPI()
{

	if (wasapInit == false) return false;


	log("Will Terminate of WASAPI...");

	if (renderer != NULL)
	{
		renderer->Shutdown();
		SafeRelease(&renderer);
		log("Released the CWASAPIRenderer class");
	}

	SafeRelease(&device);
	CoUninitialize();
	log("WASAPI is realeased and terminated");
	return true;
}


bool AudioWorkerThread::PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole)
{
	HRESULT hr;
	bool retValue = true;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;

	*IsDefaultDevice = false;   // Assume we're not using the default device.

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		log("Unable to instantiate device enumerator: ", hr);
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
			log("Unable to retrieve device collection: ", hr);
			retValue = false;
			goto Exit;
		}

		log("Available an output device(s):");
		log("    0:  Default Console Device");
		log("    1:  Default Communications Device");
		log("    2:  Default Multimedia Device");
		UINT deviceCount;
		hr = deviceCollection->GetCount(&deviceCount);
		if (FAILED(hr))
		{
			log("Unable to get device collection length: ", hr);
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

			wxString indexString = wxString::Format(wxT("%i"), i + 3);
			wxString deviceString(deviceName);

			wxString message;
			message.append("    ");
			message.append(indexString);
			message.append(":  ");
			message.append(deviceString);
			log(message);

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
			//log("unrecognized device index: ", choice);
			log("unrecognized device index: ");

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
				//log("Unable to retrieve device %d: %x\n", deviceIndex - 3, hr);
				log("Unable to retrieve device ", deviceIndex - 3);

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
			//log("Unable to get endpoint for endpoint %S: %x\n", OutputEndpoint, hr);
			//log("Unable to get endpoint for endpoint %S: %x\n", OutputEndpoint, hr);
			log("Unable to get endpoint for ...");


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
			//log("Unable to get default device for role %d: %x\n", deviceRole, hr);
			log("Unable to get default device for role...");

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



LPWSTR AudioWorkerThread::GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex)
{
	IMMDevice* device;
	LPWSTR deviceId;
	HRESULT hr;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
	{
		log("Unable to get device ", DeviceIndex);
		return NULL;
	}
	hr = device->GetId(&deviceId);
	if (FAILED(hr))
	{
		log("Unable to get device id # ", DeviceIndex);
		return NULL;
	}

	IPropertyStore* propertyStore;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	SafeRelease(&device);
	if (FAILED(hr))
	{
		log("Unable to open property store of device ", DeviceIndex);
		return NULL;
	}

	PROPVARIANT friendlyName;
	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	SafeRelease(&propertyStore);

	if (FAILED(hr))
	{
		//log("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
		log("Unable to retrieve friendly name for device ", DeviceIndex);

		return NULL;
	}

	wchar_t deviceName[128];
	hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
	if (FAILED(hr))
	{
		log("Unable to format friendly name for device ", DeviceIndex);
		return NULL;
	}

	PropVariantClear(&friendlyName);
	CoTaskMemFree(deviceId);

	wchar_t* returnValue = _wcsdup(deviceName);
	if (returnValue == NULL)
	{
		log("Unable to allocate buffer for return\n");
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
int AudioWorkerThread::generateSignal(int signalFreq, int channelCount,
	CWASAPIRenderer::RenderSampleType SampleType,
	int samplesPerSecond,
	int frameSize,
	int bufferSizeInBytes, int totNumberOfBuffers,
	int signalType,bool addNoiseIndicator)
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
	log("Will generate the data of a signal of frequency ", signalFreq);

	size_t i;
	for (i = 0; i < totNumberOfBuffers; i += 1)
	{
		RenderBuffer* renderBuffer = new RenderBuffer();

		if (renderBuffer == NULL)
		{
			log("Unable to allocate render buffer");
			return -1;
		}
		renderBuffer->_BufferLength = bufferSizeInBytes;
		renderBuffer->_Buffer = new BYTE[bufferSizeInBytes];

		if (renderBuffer->_Buffer == NULL)
		{
			log("Unable to allocate render buffer");
			return -1;
		}

		genParams.Buffer = renderBuffer->_Buffer;
		genParams.BufferLength = renderBuffer->_BufferLength;
		genParams.masterVolume = (float)MasterVolume;

		switch (SampleType)
		{
		case CWASAPIRenderer::SampleTypeFloat:

			if (signalType == SINE_SIGNAL)
				GenerateSineSamples<float>(&genParams);
			else if (signalType == TRIANGLE_SIGNAL)
				GenerateTriangleSamples<float>(&genParams);
			else if (signalType == SAW_SIGNAL)
				GenerateSawSamples<float>(&genParams);
			else if (signalType == SQUARE_SIGNAL)
				GenerateSquareSamples<float>(&genParams);
			else if (signalType == NOISE_SIGNAL)
				GenerateWhiteNoiseSamples<float>(&genParams);
			else
           		GenerateSineSamples<float>(&genParams);


			break;
		case CWASAPIRenderer::SampleType16BitPCM:
			log("CWASAPIRenderer::SampleType16BitPCM");

			if (signalType == SINE_SIGNAL)
				GenerateSineSamples<short>(&genParams);
			else if (signalType == TRIANGLE_SIGNAL)
				GenerateTriangleSamples<short>(&genParams);
			else if (signalType == SAW_SIGNAL)
				GenerateSawSamples<short>(&genParams);
			else if (signalType == SQUARE_SIGNAL)
				GenerateSquareSamples<short>(&genParams);
			else if (signalType == NOISE_SIGNAL)
				GenerateWhiteNoiseSamples<short>(&genParams);

			break;
		}

		//
		//  Link the newly allocated and filled buffer into the queue.  
		//
		*currentBufferTail = renderBuffer;
		currentBufferTail = &renderBuffer->_Next;
	}

	log("Signal Generation Completed:");
	log("   Number of buffers of data: ", i);
	log("   Count of bytes each: ", bufferSizeInBytes);
}