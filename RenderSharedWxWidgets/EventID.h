#pragma once


enum
{
    ID_TextMain = wxID_HIGHEST + 1,
    ID_InitWASAPIButton,
    ID_PlayButton,
    ID_StopButton,
    ID_InitWASAPI_Menu,
    ID_Hello,
    ID_WaveTypeRadioBox,
    ID_VolumeSlider,
    ID_FrequencySlider,

    ID_TestLog,
    ID_LogEditBox,
    ID_ClearLogButton,
    

    wxID_CHECKBOX,
    ID_SINE_WAVE_TYPE,
    ID_TRIANGLE_WAVE_TYPE,
    ID_AudioThreadEvent,
    ID_AUDIO_WORKER_EVENT_UPDATE,
    ID_AUDIO_WORKER_EVENT_COMPLETE,
    ID_AUDIO_WORKER_PLAY_IN_PROGRESS_EVENT_UPDATE,
    ID_AUDIO_WORKER_PLAY_COMPLETED_EVENT


};


enum
{
    NO_COMMAND,

    INIT_WASAPI_COMMAND,
    TERMINATE_WASAPI_COMMAND,
    PLAY_COMMAND,
    TERMINATE_AUDIO_THREAD_COMMAND,
    SET_MASTER_VOLUME,
    SET_FREQUENCY

};
