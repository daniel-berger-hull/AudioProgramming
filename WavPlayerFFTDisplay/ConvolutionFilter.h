#pragma once

#define PI 3.14159265


#define MINIMUM_CUTOFF_FREQUENCY   0.001
#define MAXIMUM_CUTOFF_FREQUENCY   1.00
#define DEFAULT_CUTOFF_FREQUENCY   0.1


/*
    Low Pass Convolution Filter, using a kernel (array of coefficent)  that is convoluted (multiplied)
    by the input signal (which is also an arraw)

    You can defined the Kernel size and the size of the input signal used in the convolution
*/
class ConvolutionFilter
{
public:
    ConvolutionFilter();
    ConvolutionFilter(int bSize, int kSize);
    ~ConvolutionFilter();



    int getInputBufferSize() { return inputBufferSize; }
    int getKernelSize() { return kernelSize; }

    void setKernel();
    void setKernel(float cutOffFrequency);

    float filter(float currentValue);


  

private:
    float* previousValues = nullptr;
    float* kernelValues = nullptr;

    int kernelSize = 1;
    int inputBufferSize = 1;
    float cutOffFrequency = 0.1f;


    // See the details in the book
    //  C:\Users\danie\OneDrive\eBooks\Electronic\DSP\Guide_to_Digital_Signal_Process.pdf
    // page 289 to 294, for  the detail of the calculation of the kernel's coefficients...
  

    void resetValues();

  
};