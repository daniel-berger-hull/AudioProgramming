#include "pch.h"
#include "WavLoader.h"



WavLoader::WavLoader()
{

}

WavLoader::~WavLoader()
{
    if (fptr != nullptr)
    {
        TRACE("File descriptior not closed, will close the stream before exit\n");
        fclose(fptr);
    }


    if (leftChannelBuffer != NULL)
    {
        TRACE("Left Channel Buffer will be deleted by destructor\n");
        delete leftChannelBuffer;
    }
    if (rightChannelBuffer != NULL)
    {
        TRACE("Right Channel Buffer will be deleted by destructor\n");
        delete rightChannelBuffer;
    }


}

/*
        A valid header for a WAVE file has the following
        Position    Size        Field       Possible Value
        ---------   ------      ------      ---------------
        0           4           ChunkID      Contains the letters "RIFF"    in ASCII form
        4           4           ChunkSize   4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
        8           4           Format       Contains the letters "WAVE"  in ASCII form


        Check site http://soundfile.sapp.org/doc/WaveFormat/ for a good overview of the header
        and other parts of the  WAVE file
*/
bool WavLoader::validateHeader()
{
   // char buffer[10];
    bool error = false;



    //for (int i = 0; i < 10; i++)  buffer[i] = 0;

   // strcpy(buffer, wavFileHeader.chunkID);
   // buffer[4] = 0;

    if (wavFileHeader.chunkID[0] != 'R' || wavFileHeader.chunkID[1] != 'I' ||
        wavFileHeader.chunkID[2] != 'F' || wavFileHeader.chunkID[3] != 'F' )
//    if (strcmp(buffer, "RIFF") != 0)
    {
        TRACE("Invalid ChunkID %s\n", wavFileHeader.chunkID);
        error = true;
    }

  //  strcpy(buffer, wavFileHeader.format);
   // buffer[4] = 0;

    //if (strcmp(buffer, "WAVE") != 0)
    if (wavFileHeader.format[0] != 'W' || wavFileHeader.format[1] != 'A' ||
        wavFileHeader.format[2] != 'V' || wavFileHeader.format[3] != 'E')
    {
        TRACE("Invalid Format %s\n", wavFileHeader.format);
        error = true;
    }


    return error;
}


bool WavLoader::validateFormatSubHeader()
{
    char buffer[10];
    bool errorFlag = false;

    //TRACE("\n*** Format Header***\n");

    strcpy(buffer, wavFileFormatSubHeader.subchunk1ID);
    buffer[4] = 0;

    if (strcmp(buffer, "fmt ") != 0)
    {
        TRACE("\tInvalid Sub ChunkID %s\n", buffer);
        errorFlag = true;
    }


    return errorFlag;
}


bool WavLoader::validateDataSubHeader()
{

    char buffer[10];
    bool errorFlag = false;

    strcpy(buffer, wavFileDataHeader.chunkID);
    buffer[4] = 0;

    if (strcmp(buffer, "data") != 0)
    {
        TRACE("\tInvalid Data Chunk %s\n", buffer);
        errorFlag = true;
    }

    return errorFlag;
}



int WavLoader::open(const char* fileName)
{
    if (fileName == NULL)
        return ERROR;

    if ((fptr = fopen(fileName, "rb")) == NULL) {
        return ERROR;
    }

    fread(&wavFileHeader, sizeof(struct _WavFileHeader), 1, fptr);
    if (validateHeader())
    {
        TRACE("Header of the WAVE file appears to be invalid, terminate problem\n");
        fclose(fptr);

        return ERROR;
    }

    fread(&wavFileFormatSubHeader, sizeof(struct _WavFileFormatSubHeader), 1, fptr);
    if (validateFormatSubHeader())
    {
        TRACE("Format Sub Header of the WAVE file appears to be invalid, terminate problem\n");
        fclose(fptr);

        return ERROR;
    }


    fread(&wavFileDataHeader, sizeof(struct _WavFileDataHeader), 1, fptr);
    if (validateDataSubHeader())
    {
        TRACE("Format Sub Header of the WAVE file appears to be invalid, terminate problem\n");
        fclose(fptr);

        return ERROR;
    }


    dislayFileInfo();


    int16_t leftChannelData = 0;
    int16_t rightChannelData = 0;

    //Will create to buffer for the left and right data (if stereo file of course)
    int bytesPerSampling = wavFileFormatSubHeader.bytePerSample >> 3;
    int bufferSize = wavFileDataHeader.chunkSize / (wavFileFormatSubHeader.numChannels* bytesPerSampling);

    leftChannelBuffer  = new int16_t[bufferSize];
    if (leftChannelBuffer == NULL)
    {
        TRACE("Allocation of left Buffer failed!\n");
    }
      
    rightChannelBuffer = new int16_t[bufferSize];
    if (rightChannelBuffer == NULL)
    {
      TRACE("Allocation of left Buffer failed!\n");
    }


//    for (int i = 0; i < 100; i++)
    int i;
    int16_t minLeftValue = 0;
    int16_t maxLeftValue = 0;
    int16_t minRightValue = 0;
    int16_t maxRightValue = 0;

    for (i = 0; i < bufferSize; i++)

    {
        fread(&leftChannelData, sizeof(int16_t), 1, fptr);
        fread(&rightChannelData, sizeof(int16_t), 1, fptr);

        leftChannelBuffer[i] = leftChannelData;
        rightChannelBuffer[i] = rightChannelData;

        if (i < 100)
            TRACE(" %d     %d\n", rightChannelData, leftChannelData);

        if (leftChannelData < minLeftValue) minLeftValue = leftChannelData;
        if (leftChannelData > maxLeftValue) maxLeftValue = leftChannelData;
        if (rightChannelData < minRightValue) minRightValue = leftChannelData;
        if (rightChannelData > maxRightValue) maxRightValue = leftChannelData;


    }

    TRACE("Reading of data completed: %d samples read in total, of %d byte(s) per samples\n",i, bytesPerSampling);
    
    TRACE("Min/Max Values:  Left [%d , % d], Right [%d , % d]\n", minLeftValue, maxLeftValue, minRightValue, maxRightValue);
    
    fclose(fptr);

    return OK;
}


void WavLoader::dislayFileInfo()
{


    if (wavFileFormatSubHeader.audioFormat = AUDIO_FORMAT_LINEAR_QUANTIZATION)
        TRACE("\tAudio Format is 1 (Linear Quantization, no compression used)\n");
    else
        TRACE("\tAudio Format is greater than 1, possible compression used in the encoding\n");


    switch (wavFileFormatSubHeader.numChannels)
    {
    case(MONO_CHANNEL):
        TRACE("\tMono Audio (1 channel)\n");
        break;
    case(STEREO_CHANNEL):
        TRACE("\tStereo Audio (2 channels)\n");
        break;
    default:
        TRACE("\tPossible problem with the number of channels\n");
        break;
    }

    if (wavFileFormatSubHeader.subchunk1Size == SUB_CHUNK_SIZE_PCM)
        TRACE("\tSubchunk1Size is -> 16 (PCM Format)\n");
    else
        TRACE("\tUnknow subchunk1Size\t ->  %d\n", wavFileFormatSubHeader.subchunk1Size);

    TRACE("\tSample Rate:\t %d Hz\n", wavFileFormatSubHeader.sampleRate);
    TRACE("\tBits per sample: %d\n", wavFileFormatSubHeader.bytePerSample);
    TRACE("\tBlock Align:\t %d\n", wavFileFormatSubHeader.blockAlign);
    TRACE("\tByte Rate:\t %d Bytes/Sec\n", wavFileFormatSubHeader.byteRate);

    TRACE("\n\tData Chunk Size: %d\n", wavFileDataHeader.chunkSize);

}

