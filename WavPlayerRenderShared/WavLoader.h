#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>



#define  OK         0
#define  ERROR     -1

//#define  TEST_WAVE_FILENAME   "D:/data/workspace/C++/DigitalSignalProcessing/FFT/FreeSmallFFT/Resources/WAV/Alarm01.wav"
//#define  TEST_WAVE_FILENAME   "D:/data/workspace/C++/DigitalSignalProcessing/FFT/FreeSmallFFT/Resources/WAV/TestSound.wav"




#define RIFF_STRING_CODE    0x52494646
#define WAVE_STRING_CODE    0x57415645


#define  SUB_CHUNK_SIZE_PCM                16
#define  AUDIO_FORMAT_LINEAR_QUANTIZATION   1

#define  MONO_CHANNEL                          1
#define  STEREO_CHANNEL                        2




struct threeNum
{
    int n1, n2, n3;
};


struct _WavFileHeader
{
    char chunkID[4];
    int chunkSize;
    char format[4];
};


struct _WavFileFormatSubHeader
{
    char subchunk1ID[4];
    int  subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint16_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bytePerSample;
};


struct _WavFileDataHeader
{
    char chunkID[4];
    int chunkSize;
};


class WavLoader
{
    public:
        WavLoader();
        ~WavLoader();

        int  open(const char* fileName);
        bool isFileOpen() { return fileOpen; }


        void dislayFileInfo();


        _WavFileHeader* getHeader() { return  &wavFileHeader; }
        _WavFileFormatSubHeader* getFormatSubHeader() { return  &wavFileFormatSubHeader; }
        _WavFileDataHeader* getDataHeader() { return  &wavFileDataHeader; }

        // Utility Getters on core information about the wav file...
        int      getSubChunkSize() { return    wavFileFormatSubHeader.subchunk1Size; }
        uint16_t getAudioFormat() { return    wavFileFormatSubHeader.audioFormat; }
        uint16_t getNumChannels() { return    wavFileFormatSubHeader.numChannels; }
        uint16_t getSampleRate() { return    wavFileFormatSubHeader.sampleRate; }
        uint32_t getByteRate() { return    wavFileFormatSubHeader.byteRate; }
        uint16_t getBlockAlign() { return    wavFileFormatSubHeader.blockAlign; }
        uint16_t getBytePerSample() { return    wavFileFormatSubHeader.bytePerSample; }
        int      getChunkSize() { return    wavFileDataHeader.chunkSize; }

        int16_t* getLeftChannelBuffer()    { return leftChannelBuffer; }
        int16_t* getRightChannelBuffer()   { return rightChannelBuffer; }



      
       

    private:
        bool validateHeader();
        bool validateFormatSubHeader();
        bool validateDataSubHeader();

        FILE* fptr = nullptr;
        bool  fileOpen = false;

        struct _WavFileHeader           wavFileHeader;
        struct _WavFileFormatSubHeader  wavFileFormatSubHeader;
        struct _WavFileDataHeader       wavFileDataHeader;


        int16_t* leftChannelBuffer = NULL;
        int16_t* rightChannelBuffer = NULL;

};

