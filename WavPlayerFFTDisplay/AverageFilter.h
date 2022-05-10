#pragma once



class AverageFilter
{
public:
    AverageFilter();
    AverageFilter(int size);
    ~AverageFilter();


    int getAverageSize() { return averageSize; }

    float filter(float currentValue);



private:
    float* previousValues = nullptr;
    int averageSize = 1;

    void resetValues();
};
