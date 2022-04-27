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
#define CURIOUS_FILTER                       4

#define MONO_CHANNEL                         1
#define STEREO_CHANNEL                       2
#define LEFT_CHANNEL                         0
#define RIGHT_CHANNEL                        1


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  This filter is a modification of the a filter from the Curiores web site at:
//       https://github.com/curiores/ArduinoTutorials/blob/main/LowPass2.0/LowPass2.0.ino
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <int order> // order is 1 or 2
class LowPass
{
private:
    float a[order];
    float b[order + 1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order + 1]; // Raw values
    float y[order + 1]; // Filtered values

public:
    LowPass(float f0, float fs, bool adaptive) {
        // f0: cutoff frequency (Hz)
        // fs: sample frequency (Hz)
        // adaptive: boolean flag, if set to 1, the code will automatically set
        // the sample frequency based on the time history.

        omega0 = 6.28318530718 * f0;
        dt = 1.0 / fs;
        adapt = adaptive;
        tn1 = -dt;
        for (int k = 0; k < order + 1; k++) {
            x[k] = 0;
            y[k] = 0;
        }
        setCoef();
    }

    void setCoef() {
        /* if (adapt) {
             float t = micros() / 1.0e6;
             dt = t - tn1;
             tn1 = t;
         }*/

        float alpha = omega0 * dt;
        if (order == 1) {
            a[0] = -(alpha - 2.0) / (alpha + 2.0);
            b[0] = alpha / (alpha + 2.0);
            b[1] = alpha / (alpha + 2.0);
        }
        if (order == 2) {
            float c1 = 2 * sqrt(2) / alpha;
            float c2 = 4 / (alpha * alpha);
            float denom = 1.0 + c1 + c2;
            b[0] = 1.0 / denom;
            b[1] = 2.0 / denom;
            b[2] = b[0];
            a[0] = -(2.0 - 2.0 * c2) / denom;
            a[1] = -(1.0 - c1 + c2) / (1.0 + c1 + c2);
        }
    }

    float filt(float xn) {
        // Provide me with the current raw value: x
        // I will give you the current filtered value: y
        if (adapt) {
            setCoef(); // Update coefficients if necessary      
        }
        y[0] = 0;
        x[0] = xn;
        // Compute the filtered values
        for (int k = 0; k < order; k++) {
            y[0] += a[k] * y[k + 1] + b[k] * x[k];
        }
        y[0] += b[order] * x[order];

        // Save the historical values
        for (int k = order; k > 0; k--) {
            y[k] = y[k - 1];
            x[k] = x[k - 1];
        }

        // Return the filtered value    
        return y[0];
    }
};




class AverageFilter
{
public:
    AverageFilter()
    {
        previousValues = new float[averageSize];
        resetValues();
    }

    AverageFilter(int size)
    {
        if (size > 1 && size < 500)
            averageSize = size;
        else
            averageSize = 3;

        previousValues = new float[averageSize];

        resetValues();
    }

    ~AverageFilter()
    {
        if (previousValues != nullptr)  delete previousValues;
    }


    int getAverageSize() { return averageSize; }

    float filter(float currentValue)
    {
        float sum = 0.0f;
        float result = 0.0f;

        // Move the previous values down, and  will place the latest values (currentValue)
        // at the end of the array previousValues;
        for (int i = 0; i < averageSize - 1; i++)
            previousValues[i] = previousValues[i + 1];

        previousValues[averageSize - 1] = currentValue;

        for (int i = 0; i < averageSize; i++)
            sum += previousValues[i];

        result = sum / (float)averageSize;

        return result;
    }

private:
    float* previousValues = nullptr;
    int averageSize = 1;

    void resetValues()
    {
        for (int i = 0; i < averageSize; i++)   previousValues[i] = 0.0f;
    }
};


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
    int cutoffFrequency;
};


AverageFilter averageFilter = AverageFilter(10);



//
//  Load samples from a pre-existing signal array and apply the filtering if required
// 
//  T:  Type of data holding the sample (short, int, byte, float)
//   GenParams struct contains the following info:
//      Buffer - Buffer to hold the samples
//      BufferLength - Length of the buffer.
//      SoundCardChannelCount - Number of channels available on the audio device
//      WavFileChannelCount - Number of channel available in the input signal (MONO vs STEREO)
//   Note: SoundCardChannelCount and WavFileChannelCount are similar but totally independent.
//         The first is about what you can physically use and available on the Sound Card (most cars have are stereo dough),
//         while    WavFileChannelCount is related to the info stored in the Wav File loaded by the program 
//      SamplesPerSecond - Samples/Second for the output data.
//      InitialTheta - Initial theta value - start at 0, modified in this function.
//      masterVolume - The overal volume  to apply to the output 0 to 100 (int value)
//      leftChannelBuffer & rightChannelBuffer - the respective destination array of output signal, for each channel
//      cutoffFrequency - If a filter is used,  this is the frequency where the filters should start to filter the signal's frequencies
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


     

     LowPass<2> lp(params->cutoffFrequency, 44100, true);




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
        else if (params->filteringType == CURIOUS_FILTER)
        {
             float current = ((float)*params->leftChannelBuffer / 32767.0f);
             float filtered = lp.filt(current);

             channelValues[LEFT_CHANNEL] = amplitude * filtered;

              dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
              dataBuffer[i + 1] = Convert<T>(channelValues[LEFT_CHANNEL]);
              params->leftChannelBuffer++;



            /*  if (params->WavFileChannelCount == MONO_CHANNEL)
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
              }*/

        }
        else if (params->filteringType == RUNNING_AVERAGE_FILTER)
          {
         

            ////////////////////////////////////////////////////////////////////////
                  if (params->WavFileChannelCount == MONO_CHANNEL)
                  {

                      float currentVal = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);
                      float filtered = averageFilter.filter(currentVal);



                      //channelValues[LEFT_CHANNEL] = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);
                      //dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                      //dataBuffer[i + 1] = Convert<T>(channelValues[LEFT_CHANNEL]);


                      dataBuffer[i] = Convert<T>(filtered);
                      dataBuffer[i + 1] = Convert<T>(filtered);


                      params->leftChannelBuffer++;
                  }
                  else if (params->WavFileChannelCount == STEREO_CHANNEL)
                  {
                      float currentVal = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);
                      float filtered = averageFilter.filter(currentVal);
                      dataBuffer[i] = Convert<T>(filtered);
                      dataBuffer[i + 1] = Convert<T>(filtered);
                      
                      //channelValues[LEFT_CHANNEL] = amplitude * ((float)*params->leftChannelBuffer / 32767.0f);
                      //dataBuffer[i] = Convert<T>(channelValues[LEFT_CHANNEL]);
                      //channelValues[RIGHT_CHANNEL] = amplitude * ((float)*params->rightChannelBuffer / 32767.0f);
                      //dataBuffer[i + 1] = Convert<T>(channelValues[RIGHT_CHANNEL]);
                      params->leftChannelBuffer++;
                      params->rightChannelBuffer++;
                  }


                  //////////////////////////////////////////////////////////////////////////////




              
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

