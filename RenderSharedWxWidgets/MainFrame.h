#pragma once

#include <wx/wx.h>



class LogFrame;
class AudioWorkerThread;


class MainFrame : public wxFrame
{

public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainFrame();



    wxButton* playButton;
    wxButton* stopButton;
	wxButton* initWASAPIButton;
    wxSlider* volumeSlider;
    wxSlider* frequencySlider;

    void setLogFrame(LogFrame* ptrFrame) {  logFrame = ptrFrame; };

private:

    wxMenuBar*   buildMenuBar();
    wxPanel*     buildMainPanel();
    wxStaticBox* buildVolumeSection(wxPanel* parent);
    wxStaticBox* buildFrequencySection(wxPanel* parent);
    wxStaticBox* buildButtonsSection(wxPanel* parent);

	void startAudioWorkerThread();


	void log(wxString message);
	void log(wxString message, int value);


    void OnPaint(wxPaintEvent&);

    void OnExitMenuItemClick(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnInitWASAPIClick(wxCommandEvent& event);
    void OnPlayButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);
	
    void OnRadioBoxChange(wxCommandEvent& evt);
    void OnCheckBoxEvent(wxCommandEvent& evt);

    void OnVolumeSliderScroll(wxScrollEvent& event);
    void OnFrequencySliderScroll(wxScrollEvent& event);

	void OnAudioWorkerPlayInProgressEventUpdate(wxThreadEvent& event);
	void OnAudioWorkerPlayCompletedEvent(wxThreadEvent& event);

    void togglePlayStopButtons(bool playing);

	AudioWorkerThread* audioThread;

	wxStaticText* volumeText;
	wxStaticText* frequencyText;

	wxRadioBox* m_radioBox;

	LogFrame* logFrame;

    int  masterVolume = 100;
    int  frequency = 440;
    int  signalType;
	bool m_audioWorkerThreadRunning = false;
    bool m_audioWorkerPlaying       = false;


    wxDECLARE_EVENT_TABLE();

};

