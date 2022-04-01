// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
#pragma once
#include <MMDeviceAPI.h>
//#include <mmdeviceapi.h>
#include <AudioClient.h>
#include <AudioPolicy.h>



// Remove comment and recompile if you want more debug output in the standard console
//#define DEBUG_WASAPI_RENDER


struct RenderBuffer
{
    RenderBuffer *  _Next;
    UINT32          _BufferLength;
    BYTE *          _Buffer;


    RenderBuffer() :
        _Next(NULL),
        _BufferLength(0),
        _Buffer(NULL)
    {
    }

    ~RenderBuffer()
    {
        delete [] _Buffer;
    }

   

    template <typename T>
    void displayFloat()
    {
        
        T* dataBuffer = reinterpret_cast<T*>(_Buffer);

        for (int i = 0; i < 400; i+=2)
        {
            printf("%f\n",dataBuffer[i]);
        }
    }



};

class CWASAPIRenderer : IMMNotificationClient, IAudioSessionEvents
{
public:
    //  Public interface to CWASAPIRenderer.
    enum RenderSampleType
    {
        SampleTypeFloat,
        SampleType16BitPCM,
    };

    CWASAPIRenderer(IMMDevice *Endpoint, bool EnableStreamSwitch, ERole EndpointRole);
    bool Initialize(UINT32 EngineLatency);
    bool IsInitiated()                  { return initiated;  }
    void Shutdown();
    bool Start(RenderBuffer *RenderBufferQueue);
    void Stop();
    WORD ChannelCount() { return _MixFormat->nChannels; }
    UINT32 SamplesPerSecond() { return _MixFormat->nSamplesPerSec; }
    UINT32 BytesPerSample() { return _MixFormat->wBitsPerSample / 8; }
    RenderSampleType SampleType() { return _RenderSampleType; }
    UINT32 FrameSize() { return _FrameSize; }
    UINT32 BufferSize() { return _BufferSize; }
    UINT32 BufferSizePerPeriod();
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    void    setRefreshDisplayEvent(HANDLE ghEvent) { ghRefreshDisplayEvent = ghEvent; }

    //void   setSourceDataBuffer(BYTE** _buffer);

    // This method has to be called to the class who need the data to be copied
    // to one of its array, to perform data processing, like FFT
    //void   assignRemoteDestDataBuffer(BYTE** remoteBufferPrtPtr) {  _RemoteBufferPtr = remoteBufferPrtPtr;   }
    //void   assignAvailableDataSize(int* ptr) { sizePtr = ptr; }



    void   setDestinationDataBuffer(BYTE* _buffer) { _RemoteBuffer = _buffer; }


   

private:
    ~CWASAPIRenderer(void);
    bool    initiated = false;
    LONG    _RefCount;
    //
    //  Core Audio Rendering member variables.
    //

    IMMDevice * _Endpoint;
    IAudioClient *_AudioClient;
    IAudioRenderClient *_RenderClient;

    HANDLE      _RenderThread;
    HANDLE      _ShutdownEvent;
    WAVEFORMATEX *_MixFormat;
    UINT32      _FrameSize;
    RenderSampleType _RenderSampleType;
    UINT32      _BufferSize;
    LONG        _EngineLatencyInMS;

    //
    //  Render buffer management.
    //
    RenderBuffer *_RenderBufferQueue;

    HANDLE  ghRefreshDisplayEvent;
    //BYTE** _RemoteBufferPtr = NULL;

    BYTE*  _RemoteBuffer = NULL;

    //int*    sizePtr = NULL;

    static DWORD __stdcall WASAPIRenderThread(LPVOID Context);
    //DWORD CWASAPIRenderer::DoRenderThread();
    DWORD DoRenderThread();

    //
    //  Stream switch related members and methods.
    //
    bool                    _EnableStreamSwitch;
    ERole                   _EndpointRole;
    HANDLE                  _StreamSwitchEvent;          // Set when the current session is disconnected or the default device changes.
    HANDLE                  _StreamSwitchCompleteEvent;  // Set when the default device changed.
    IAudioSessionControl *  _AudioSessionControl;
    IMMDeviceEnumerator *   _DeviceEnumerator;
    bool                    _InStreamSwitch;

    bool InitializeStreamSwitch();
    void TerminateStreamSwitch();
    bool HandleStreamSwitchEvent();

    STDMETHOD(OnDisplayNameChanged) (LPCWSTR /*NewDisplayName*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnIconPathChanged) (LPCWSTR /*NewIconPath*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnSimpleVolumeChanged) (float /*NewSimpleVolume*/, BOOL /*NewMute*/, LPCGUID /*EventContext*/) { return S_OK; }
    STDMETHOD(OnChannelVolumeChanged) (DWORD /*ChannelCount*/, float /*NewChannelVolumes*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
    STDMETHOD(OnGroupingParamChanged) (LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) {return S_OK; };
    STDMETHOD(OnStateChanged) (AudioSessionState /*NewState*/) { return S_OK; };
    STDMETHOD(OnSessionDisconnected) (AudioSessionDisconnectReason DisconnectReason);
    STDMETHOD(OnDeviceStateChanged) (LPCWSTR /*DeviceId*/, DWORD /*NewState*/) { return S_OK; }
    STDMETHOD(OnDeviceAdded) (LPCWSTR /*DeviceId*/) { return S_OK; };
    STDMETHOD(OnDeviceRemoved) (LPCWSTR /*DeviceId(*/) { return S_OK; };
    STDMETHOD(OnDefaultDeviceChanged) (EDataFlow Flow, ERole Role, LPCWSTR NewDefaultDeviceId);
    STDMETHOD(OnPropertyValueChanged) (LPCWSTR /*DeviceId*/, const PROPERTYKEY /*Key*/){return S_OK; };

    //
    //  IUnknown
    //
    STDMETHOD(QueryInterface)(REFIID iid, void **pvObject);

    //
    //  Utility functions.
    //
    bool CalculateMixFormatType();
    bool InitializeAudioEngine();
    bool LoadFormat();

    //bool test();
};
