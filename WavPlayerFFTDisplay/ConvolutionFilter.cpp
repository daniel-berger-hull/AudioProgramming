

#include "pch.h"
#include "ConvolutionFilter.h"

#include <math.h>



 ConvolutionFilter::ConvolutionFilter()
 {
        previousValues = new float[inputBufferSize];
        kernelValues = new float[kernelSize];

        setKernel();
        resetValues();
 }

 ConvolutionFilter::ConvolutionFilter(int bSize, int kSize)
 {
     if (bSize > 1 && bSize < 500)
         inputBufferSize = bSize;
     else
         inputBufferSize = 3;

     if (kSize > 1 && kSize < 100)
         kernelSize = kSize;
     else
         kernelSize = 3;

     previousValues = new float[inputBufferSize];
     kernelValues = new float[kernelSize];

     resetValues();
     setKernel();
 }

ConvolutionFilter::~ConvolutionFilter()
{
    if (previousValues != nullptr)  delete previousValues;
    if (kernelValues != nullptr)    delete kernelValues;
}



float ConvolutionFilter::filter(float currentValue)
{
     float sum = 0.0f;
     float result = 0.0f;

     // Move the previous values down, and  will place the latest values (currentValue)
     // at the end of the array previousValues;
     //    **** Use a circular buffer here instead!!!
     for (int i = 0; i < inputBufferSize - 1; i++)
         previousValues[i] = previousValues[i + 1];

     previousValues[inputBufferSize - 1] = currentValue;

     int inputIndex = inputBufferSize - 1;
     for (int i = 0; i < kernelSize; i++)
     {
         sum += previousValues[inputIndex] * kernelValues[i];
         inputIndex--;
     }

     result = sum / (float)kernelSize;

     return result;
 }


void ConvolutionFilter::setKernel(float cutOffFrequency)
{

    if (cutOffFrequency < MINIMUM_CUTOFF_FREQUENCY)        cutOffFrequency = MINIMUM_CUTOFF_FREQUENCY;
    else  if (cutOffFrequency > MAXIMUM_CUTOFF_FREQUENCY)  cutOffFrequency = MAXIMUM_CUTOFF_FREQUENCY;

    TRACE("Kernel Values: Initial pass...\n");
    int k = kernelSize / 2;
    double factor = 2.0f * PI * cutOffFrequency;
    for (int i = 0; i < kernelSize; i++)
    {
        double delayedValue = (double)i - (double)k;

        if (i != k)
            kernelValues[i] = (float)(sin(factor * delayedValue) / delayedValue);
        else
            kernelValues[i] = (float)(factor);


        TRACE("%i --> %f\n", i, kernelValues[i]);
    }

    // See p285 of the Guide_to_Digital_Signal_Process book for details on the
    // Hamming Window equations to determined the coefficient of the filter
    TRACE("Kernel Values: Hamming Window calculation...\n");
    for (int i = 0; i < kernelSize; i++)
    {
        float windowFactor = 0.54f - (0.46 * cos(2.0f * PI * (float)i / (float)kernelSize));
        kernelValues[i] = kernelValues[i] * windowFactor;
        TRACE("%i --> %f\n", i, kernelValues[i]);
    }


    float kernelSum = 0.0f;
    for (int i = 0; i < kernelSize; i++)
        kernelSum += kernelValues[i];

    TRACE("Kernel Values: kernel normalization calculation...\n");
    for (int i = 0; i < kernelSize; i++)
    {
        kernelValues[i] = (float)kernelSize * kernelValues[i] / kernelSum;
        TRACE("%i --> %f\n", i, kernelValues[i]);
    }
}




    // See the details in the book
    //  C:\Users\danie\OneDrive\eBooks\Electronic\DSP\Guide_to_Digital_Signal_Process.pdf
    // page 289 to 294, for  the detail of the calculation of the kernel's coefficients...
void ConvolutionFilter::setKernel()
 {
    setKernel(DEFAULT_CUTOFF_FREQUENCY);
 }

void ConvolutionFilter::resetValues()
{
     for (int i = 0; i < inputBufferSize; i++)   previousValues[i] = 0.0f;
}
