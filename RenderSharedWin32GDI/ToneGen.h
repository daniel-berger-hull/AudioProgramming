// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//

//
//  Sine tone generator.
//
#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


template<typename T> T Convert(double Value);

struct  WaveParams
{
    double thetas[10] = { 0.0, 0.0 , 0.0, 0.0,  0.0 , 0.0, 0.0 , 0.0, 0.0,  0.0 };
    double deltaT[10];
    double amplitude[10] = { 0.5, 0.6, 0.21, 0.13, 0.1 , 0.8, 0.6, 0.5, 0.45, 0.4 };
    double amplitudeSum = 0.0;

    int nbrFrequencies = 10;
};

struct  GenParams
{
    BYTE*      Buffer;
    size_t     BufferLength;
    DWORD      Frequency;
    WORD       ChannelCount;
    DWORD      SamplesPerSecond;
    double*    InitialTheta;
    double*    carryOverValue;
    
    WaveParams* waveParams;
};



struct ADSR
{
    int attack;
    int decay;
    int sustain;
    int release;   
};


void initValuesSamples(WaveParams* waveParams, DWORD Frequency, DWORD SamplesPerSecond)
{
    for (int i = 0; i < waveParams->nbrFrequencies; i++)
    {
        waveParams->deltaT[i] = (Frequency * (i + 1) * (M_PI * 2)) / (double)SamplesPerSecond;
        waveParams->amplitudeSum += waveParams->amplitude[i];
    }

    waveParams->amplitudeSum /= 2;
}




//
//  Generate samples which represent a sine wave that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//  Buffer - Buffer to hold the samples
//  BufferLength - Length of the buffer.
//  ChannelCount - Number of channels per audio frame.
//  SamplesPerSecond - Samples/Second for the output data.
//  InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void GenerateSineSamples(GenParams* params)
{
    double sampleIncrement = (params->Frequency * (M_PI*2)) / (double)params->SamplesPerSecond;
    T *dataBuffer = reinterpret_cast<T *>(params->Buffer);
    double theta = (params->InitialTheta != NULL ? *(params->InitialTheta) : 0);
    float outputValue = 0.0f;
    float amplitude = (MasterVolume / 100.0f);
    
    for (size_t i = 0 ; i < params->BufferLength / sizeof(T) ; i += params->ChannelCount)
    {        
        outputValue = amplitude * sin(theta);
        for(size_t j = 0 ;j < params->ChannelCount; j++)
        {
            dataBuffer[i + j] = Convert<T>(outputValue);
        }

        theta += sampleIncrement;
    }

    if (params->InitialTheta != NULL)
    {
        *(params->InitialTheta) = theta;
    }
    
}





//
//  Generate samples which represent a sine wave that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//  Buffer - Buffer to hold the samples
//  BufferLength - Length of the buffer.
//  ChannelCount - Number of channels per audio frame.
//  SamplesPerSecond - Samples/Second for the output data.
//  InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void GenerateTriangleSamples(GenParams* params)
{
    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);

    double samplesBySignalCycle = (double)params->SamplesPerSecond / (double)params->Frequency;  // How many samples done the sample frequency (SamplesPerSecond)
    double sampleIncrement      = (params->Frequency * (M_PI * 2)) / (double)params->SamplesPerSecond;
//    double slope                = (1.0 / (M_PI / 2.0)) / (samplesBySignalCycle/4.0);
    //  Problem with the slope, and the 1.33 factor is only a patch...
    //  Not sure why it is needed, but is you don't the signal never reach the value 1.0f
    double slope = (1.0 / (M_PI / 2.0)) / (samplesBySignalCycle / 4.0) * 1.33;

    double currentSlope         = 0.0f;


    double theta         = (params->InitialTheta != NULL ? *(params->InitialTheta) : 0);
    double currentValue  = (params->carryOverValue != NULL ? *(params->carryOverValue) : 0);


    float step1 = M_PI / 2.0f;
    float step2 = (3.0f * M_PI) / 2.0f;
    float TWO_PI = 2.0f * M_PI;
  
    float outputValue = 0.0f;
    float amplitude = (MasterVolume / 100.0f);

    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->ChannelCount)
    {

        if (theta < step1)         currentSlope = slope;
        else  if (theta < step2)   currentSlope = -slope;
        else                       currentSlope = slope;

        currentValue += currentSlope;
        outputValue = amplitude * currentValue;


        for (size_t j = 0; j < params->ChannelCount; j++)
        {
            dataBuffer[i + j] = Convert<T>(outputValue);
        }

        theta += sampleIncrement;
        if (theta > TWO_PI)  
        {
            theta = 0.0f;
            currentValue = 0.0f;
        }
    }


    if (params->InitialTheta != NULL)
    {
        *(params->InitialTheta) = theta;
    }


    if (params->carryOverValue != NULL)
    {
        *(params->carryOverValue) = currentValue;
    }
}

//
//  Generate samples which represent a sine wave that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//  Buffer - Buffer to hold the samples
//  BufferLength - Length of the buffer.
//  ChannelCount - Number of channels per audio frame.
//  SamplesPerSecond - Samples/Second for the output data.
//  InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void GenerateSawSamples(GenParams* params)
{

    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);

    float samplesBySignalCycle = (float)params->SamplesPerSecond / (float)params->Frequency;  // How many samples done the sample frequency (SamplesPerSecond)                                                                                 // can fit within a full cycle of the signal itself?
    float slope = (1.0 / samplesBySignalCycle) * 0.5;
    float currentValue = (params->carryOverValue != NULL ? *(params->carryOverValue) : 0);
    float outputValue = 0.0f;
    float amplitude = (MasterVolume / 100.0f);

    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->ChannelCount)
    {       
        currentValue += slope;
        if (currentValue >= 1.0)   currentValue = -1.0;

        outputValue = amplitude * currentValue;   
        for (size_t j = 0; j < params->ChannelCount; j++)
        {
            dataBuffer[i + j] = Convert<T>(outputValue);            
        }
    }

  
    if (params->carryOverValue != NULL)
    {
        *(params->carryOverValue) = currentValue;
    }
}


//
//  Generate samples which represent a white noise that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//  Buffer - Buffer to hold the samples
//  BufferLength - Length of the buffer.
//  ChannelCount - Number of channels per audio frame.
//  SamplesPerSecond - Samples/Second for the output data.
//  InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void GenerateWhiteNoiseSamples(GenParams* params)
{
    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);
    float outputValue = 0.0f;
    float amplitude = (MasterVolume / 100.0f);

    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->ChannelCount)
    {
        outputValue = amplitude * (rand() / (double)RAND_MAX);
        for (size_t j = 0; j < params->ChannelCount; j++)
        {
            dataBuffer[i + j] = Convert<T>(outputValue);
        }
    }
}


//
//  Generate samples which represent a sine wave of many different frequencies that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//  Buffer - Buffer to hold the samples
//  BufferLength - Length of the buffer.
//  ChannelCount - Number of channels per audio frame.
//  SamplesPerSecond - Samples/Second for the output data.
//  InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void GenerateMultiSineSamples(BYTE* Buffer, size_t BufferLength, DWORD BaseFrequency, WORD ChannelCount, DWORD SamplesPerSecond, WaveParams* waveParams)
{
    T* dataBuffer = reinterpret_cast<T*>(Buffer);

    for (size_t i = 0; i < BufferLength / sizeof(T); i += ChannelCount)
    {
        double sinValue = 0.0;
        for (int i = 0; i < waveParams->nbrFrequencies; i++)
        {
            double tmpValue = waveParams->amplitude[i] * sin(waveParams->thetas[i]);
            waveParams->thetas[i] += waveParams->deltaT[i];
            sinValue += tmpValue;
        }
        

        for (size_t j = 0; j < ChannelCount; j++)
        {
            dataBuffer[i + j] = Convert<T>(sinValue);
        }
    }
}











//
//  Convert from double to float, byte, short or int32.
//
template<> 
float Convert<float>(double Value)
{
    return (float)(Value);
};

template<> 
short Convert<short>(double Value)
{
    return (short)(Value * _I16_MAX);
};

