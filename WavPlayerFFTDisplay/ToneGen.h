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
void LoadWavSamples(GenParams* params, int16_t* leftChannelBuffer, int16_t* rightChannelBuffer, int filteringType)
{
    T* dataBuffer = reinterpret_cast<T*>(params->Buffer);
    float leftChannelValue = 0.0f;
    float rightChannelValue = 0.0f;

    float amplitude = (params->masterVolume / 100.0f);

    /// following section is required for the filtering 
    //  (if required by the value of the param 'filtering')
    float previousLeftValue = ((float)*leftChannelBuffer / 32767.0f);
    float previousRightValue = ((float)*rightChannelBuffer / 32767.0f);
    float currentLeftValue = 0.0f;
    float currentRightValue = 0.0f;
    float previousLeftFilteredValue = previousLeftValue;
    float previousRightFilteredValue = previousRightValue;


    


    for (size_t i = 0; i < params->BufferLength / sizeof(T); i += params->ChannelCount)
    {

        if (filteringType == SIMPLE_FILTER)
        {
            leftChannelBuffer++;
            rightChannelBuffer++;

            /*currentLeftValue = amplitude * ((float)*leftChannelBuffer / 32767.0f);
            currentRightValue = amplitude * ((float)*rightChannelBuffer / 32767.0f);*/
            currentLeftValue =  ((float)*leftChannelBuffer / 32767.0f);
            currentRightValue = ((float)*rightChannelBuffer / 32767.0f);

           /* float summationLeftValue = (0.828 * previousLeftFilteredValue) +
                (0.0828 * currentLeftValue) +
                (0.0828 * previousLeftValue);

            float summationRightValue = (0.828 * previousRightFilteredValue) +
                (0.0828 * currentRightValue) +
                (0.0828 * previousRightValue);*/
            float summationLeftValue = (0.969 * previousLeftFilteredValue) +
                (0.0155 * currentLeftValue) +
                (0.0155 * previousLeftValue);

            float summationRightValue = (0.969 * previousRightFilteredValue) +
                (0.0155 * currentRightValue) +
                (0.0155 * previousRightValue);




            leftChannelValue = amplitude * summationLeftValue;
            rightChannelValue = amplitude * summationRightValue;


            previousLeftValue = currentLeftValue;
            previousRightValue = currentRightValue;

            previousLeftFilteredValue = summationLeftValue;
            previousRightFilteredValue = summationRightValue;
        }
        else
        {
              leftChannelValue = amplitude * ( (float)*leftChannelBuffer / 32767.0f);
              rightChannelValue = amplitude *( (float)*rightChannelBuffer / 32767.0f);
              leftChannelBuffer++;
              rightChannelBuffer++;
        }

    
        dataBuffer[i] = Convert<T>(leftChannelValue);
        dataBuffer[i + 1] = Convert<T>(rightChannelValue);

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

