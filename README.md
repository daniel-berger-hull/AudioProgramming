June 2022
---------

  Note that the framework to be used for the next projects will be wxWidgets, as there is still support for this library
  (unlike Microsoft Win32 and MFC as the community has shifted to the C# development in the last decade)
  

# AudioProgramming
==================

Collection of audio and tone generation projects in C++ running on Windows and built using Microsoft Visual Studio 2019 (Community edition)



RenderSharedWin32GDI
--------------------

Tone generation project, using Window's WASAPI library, as described in the Windows classic(multimedia)
code examples. The application is a basic Win32 project, and can generate few signal type (sine, triangle,
saw and noise), and also has a master volume


RenderSharedMFC
---------------

This is basically an MS MFC version of the RenderSharedWin32GDI. It contains also few more feature, like
a square wave signal, and an experiment of a multiple frequency (sine wave) signal, for which, there is 
slider controls to modify the amplitude of the first 4 harmonic of the waves that are building the signal.


WavPlayerRenderShared
---------------------

A project to render the content of a wav file, using the WASAPI and MFC skeleton used in the RenderSharedMFC.


FFTDisplay
----------

 Basic FFT done on a signal and display frequency spectrum with a bar graph.
 

WavPlayerFFTDisplay
-------------------

This is a modified version of the WavPlayerRenderShared project, where an FFT calculation and display is added.
The CaptureAudioFFT project been helping acheiving this task (but this project is about output wave while
CaptureAudioFFT is about recording).


RenderSharedWxWidgets
---------------------

This is a remake of the RenderSharedWin32GDI and RenderSharedMFC using the graphic library wxWidgets instead of Win32/MFC.


CaptureAudioFFT
---------------

A very interesting project of FFT applied to audio capture taken from the Web.
This is a very good study case to understand the application of FFT in signals. 