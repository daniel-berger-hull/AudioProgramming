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



#define NO_FILTER							 0
#define SIMPLE_FILTER						 1
#define RUNNING_AVERAGE_FILTER				 2
#define SMOOTH_OPERATOR_FILTER				 3

#define MONO_CHANNEL                         1
#define STEREO_CHANNEL                       2
#define LEFT_CHANNEL                         0
#define RIGHT_CHANNEL                        1


struct  GenParams
{
    float      masterVolume;

    BYTE*      Buffer;
    size_t     BufferLength;
    WORD       SoundCardChannelCount;
    WORD       WavFileChannelCount;

    DWORD      SamplesPerSecond;
   
    // Output params
    int16_t* leftChannelBuffer;
    int16_t* rightChannelBuffer; 
    int filteringType;
};





//
//  Generate samples which represent a sine wave that fits into the specified buffer.  
//
//  T:  Type of data holding the sample (short, int, byte, float)
//   GenParams struct contains the following info:
//      Buffer - Buffer to hold the samples
//      BufferLength - Length of the buffer.
//      ChannelCount - Number of channels per audio frame.
//      SamplesPerSecond - Samples/Second for the output data.
//      InitialTheta - Initial theta value - start at 0, modified in this function.
//
template <typename T>
void LoadWavSamples(GenParams* params)
{
    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);

    const int nbrChannels = (int)params->SoundCardChannelCount;
    
    float amplitude = (params->masterVolume / 100.0f);
    float channelValues[2];
    float previousChannelValues[2];
    float currentChannelValues[2];
    float previousFilteredValue[2];
    


    for (int i = 0; i < params->SoundCardChannelCount; i++)
    {
        channelValues[i] = currentChannelValues[i] = 0.0f;
        previousFilteredValue[i] = 0.0f;
    }
    previousChannelValues[LEFT_CHANNEL] = ((float)*params->leftChannelBuffer / 32767.0f);


     if (params->WavFileChannelCount == STEREO_CHANNEL)
     {
        previousChannelValues[RIGHT_CHANNEL] = ((float)*params->rightChannelBuffer / 32767.0f);
     }
            
    /// following section is required for the filtering 
    //  (if required by the value of the param 'filtering')
    //float leftChannelValue = 0.0f;
    //float rightChannelValue = 0.0f;
    //float previousLeftValue = ((float)*leftChannelBuffer / 32767.0f);
    //float previousRightValue = ((float)*rightChannelBuffer / 32767.0f);
   /* float currentLeftValue = 0.0f;
    float currentRightValue = 0.0f;*/



    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->SoundCardChannelCount)
    {

          if (params->filteringType == SIMPLE_FILTER)
          { 
            params->leftChannelBuffer++;
            currentChannelValues[LEFT_CHANNEL] = ((float)*params->leftChannelBuffer / 32767.0f);
            float summationLeftValue = (0.969 * previousFilteredValue[LEFT_CHANNEL]) +
                (0.0155 * currentChannelValues[LEFT_CHANNEL]) +
                (0.0155 * previousChannelValues[LEFT_CHANNEL]);
           

            channelValues[LEFT_CHANNEL]         = amplitude * summationLeftValue;
            previousChannelValues[LEFT_CHANNEL] = currentChannelValues[LEFT_CHANNEL];
            previousFilteredValue[LEFT_CHANNEL] = summationLeftValue;

            if (params->WavFileChannelCount == MONO_CHANNEL)
            {               
                dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                dataBuffer[i + 1] = Convert<T>(channelValues[LEFT_CHANNEL]);
            }
            else if (params->WavFileChannelCount == STEREO_CHANNEL)
            {
                dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                dataBuffer[i + 1] = Convert<T>(channelValues[LEFT_CHANNEL]);
            }
        }
        else
        {
            if (params->WavFileChannelCount == MONO_CHANNEL)
            {
                channelValues[LEFT_CHANNEL] = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);

                dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                dataBuffer[i + 1] = Convert<T>(channelValues[LEFT_CHANNEL]);
                params->leftChannelBuffer++;
            }
            else if (params->WavFileChannelCount == STEREO_CHANNEL)
            {
                channelValues[LEFT_CHANNEL] = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);
                dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                channelValues[RIGHT_CHANNEL] = amplitude * ((float)*params->rightChannelBuffer / 32767.0f);
                dataBuffer[i + 1] = Convert<T>(channelValues[RIGHT_CHANNEL]);
                params->leftChannelBuffer++;
                params->rightChannelBuffer++;
            }
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

