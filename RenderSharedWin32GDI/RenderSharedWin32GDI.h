#pragma once

#include <iostream>
#include <fstream>

#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"

#include <functiondiscoverykeys.h>

#include "ToneGen.h"
#include "WASAPIRenderer.h"



#define SCREEN_X_POS					800
#define SCREEN_Y_POS					100
#define SCREEN_WIDTH					600
#define SCREEN_HEIGHT					400

#define DEFAULT_LABEL_WIDTH              30
#define DEFAULT_LABEL_HEIGHT             20
#define DEFAULT_COMMAND_BUTTON_WIDTH     80
#define DEFAULT_COMMAND_BUTTON_HEIGHT    30


#define MINIMAL_FREQUENCY                 220
#define MAXMIMAL_FREQUENCY                880
#define MINIMAL_VOLUME                      0
#define MAXMIMAL_VOLUME                    10


#define  SINE_SIGNAL                         1
#define  COMPLEX_SINE_SIGNAL                 2
#define  TRIANGLE_SIGNAL                     3
#define  SAW_SIGNAL                          4
#define  SQUARE_SIGNAL                       5
#define  WHITE_NOISE_SIGNAL                  6




HWND hwndButtonPlay;
HWND hwndButtonStop;

HWND hSineWaveRadioButton;
HWND hSawWaveRadioButton;
HWND hTriangleWaveRadioButton;
HWND hSquareWaveRadioButton;
HWND hNoiseWaveRadioButton;


HWND hFrequencyTrack;
HWND hFrequencyValueLabel;
HWND hFrequencyLeftLabel;
HWND hFrequencyRightLabel;

HWND hVolumeTrack;
HWND hVolumeValueLabel;
HWND hVolumeLeftLabel;
HWND hVolumeRightLabel;


HWND hwndTestButton;


HANDLE hPlayThread = NULL;
DWORD dwTPlayhreadID;


CWASAPIRenderer* renderer = NULL;
RenderBuffer* renderQueue = NULL;

int MasterVolume = 100;
int TargetFrequency = 440;
int TargetLatency = 50;
int TargetDurationInSec = 2;
bool ShowHelp;
bool UseConsoleDevice;
bool UseCommunicationsDevice;
bool UseMultimediaDevice;
bool DisableMMCSS;
int  signalType = SINE_SIGNAL;


wchar_t* OutputEndpoint;

int result = 0;
IMMDevice* device = NULL;
bool isDefaultDevice;
ERole role;

UINT32 renderBufferSizeInBytes;
size_t renderDataLength;
size_t renderBufferCount;


#define REFTIMES_PER_SEC		10000000
#define REFTIMES_PER_MILLISEC	10000






//void  createUserControls(HWND hWnd);
void  createUserForm(HWND hWnd);
void  buildButtons(HWND hWnd);
void  buildRadioSection(HWND hWnd);
void  buildFrequencyTracker(HWND hWnd);
void  buildVolumeTracker(HWND hWnd);




void  UpdateFrequencyLabel(void);

int generateSignal(int signalFreq, int channelCount, CWASAPIRenderer::RenderSampleType SampleType,
                   int samplesPerSecond,int frameSize, int bufferSizeInBytes, int totNumberOfBuffers);
bool playTone(int freq, int durationInSec);


LPWSTR  GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex);
LPWSTR  GetDeviceCompleteName(IMMDevice* device);
LPWSTR  getMixFormatWaveString(WAVEFORMATEX* wf);
bool    PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole);
DWORD WINAPI PlayToneThreadProc(LPVOID lpParam);

void testSignals();




typedef struct PlayThreadParams {
    int frequency;
    int durationInSec;
} PLAY_THREAD_PARAMS, * PPLAY_THREAD_PARAMS;

PPLAY_THREAD_PARAMS params = NULL;