# AudioProgramming
Collection of audio and tone generation projects in C++ running on Windows



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

CaptureAudioFFT
---------------

A very interesting project of FFT applied to audio capture taken from the Web.
This is a very good study case to understand the application of FFT in signals. 