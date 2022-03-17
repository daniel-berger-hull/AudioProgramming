// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//


#pragma once

#include "pch.h"
#include "framework.h"


#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


template<typename T> T Convert(double Value);







struct  GenParams
{
    BYTE*      Buffer;
    size_t     BufferLength;
    WORD       ChannelCount;
    DWORD      SamplesPerSecond;
    float      masterVolume;
};




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
void LoadWavSamples(GenParams* params, int16_t* leftChannelBuffer, int16_t* rightChannelBuffer)
{
    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);
    float leftChannelValue = 0.0f;
    float rightChannelValue = 0.0f;

    float amplitude = (params->masterVolume / 100.0f);

    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->ChannelCount)
    {
        leftChannelValue = amplitude * ( (float)*leftChannelBuffer / 32767.0f);
        rightChannelValue = amplitude *( (float)*rightChannelBuffer / 32767.0f);

        dataBuffer[i] = Convert<T>(leftChannelValue);
        dataBuffer[i + 1] = Convert<T>(rightChannelValue);
        leftChannelBuffer++;
        rightChannelBuffer++;
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

