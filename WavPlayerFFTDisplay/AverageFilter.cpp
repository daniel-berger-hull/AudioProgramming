
#include "pch.h"
#include "AverageFilter.h"



AverageFilter::AverageFilter()
{
    previousValues = new float[averageSize];
    resetValues();
}

AverageFilter::AverageFilter(int size)
{
    if (size > 1 && size < 500)
        averageSize = size;
    else
        averageSize = 3;

    previousValues = new float[averageSize];

    resetValues();
}

AverageFilter::~AverageFilter()
{
    if (previousValues != nullptr)  delete previousValues;
}

float AverageFilter::filter(float currentValue)
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


void AverageFilter::resetValues()
{
    for (int i = 0; i < averageSize; i++)   previousValues[i] = 0.0f;
}
