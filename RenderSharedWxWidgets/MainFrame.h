#pragma once

#include <wx/wx.h>


class MainFrame : public wxFrame
{

public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    wxButton* playButton;
    wxButton* stopButton;

    wxSlider* volumeSlider;
    wxSlider* frequencySlider;

    
    wxTextCtrl* mainEditBox;



private:

    wxMenuBar* buildMenuBar();
    wxPanel* buildMainPanel();

    wxStaticBox* buildVolumeSection(wxPanel* parent);
    wxStaticBox* buildFrequencyVolumeSection(wxPanel* parent);
    wxStaticBox* buildButtonsSection(wxPanel* parent);



    wxStaticText* volumeText;
    wxStaticText* frequencyText;


    wxRadioBox* m_radioBox;

    void OnPaint(wxPaintEvent&);

    void OnHello(wxCommandEvent& event);
    void OnOpenFile(wxCommandEvent& event);
    void OnExitMenuItemClick(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnPlayButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);

    void OnRadioBoxChange(wxCommandEvent& evt);
    void OnCheckBoxEvent(wxCommandEvent& evt);

    void OnVolumeSliderScroll(wxScrollEvent& event);
    void OnFrequencySliderScroll(wxScrollEvent& event);


    int clickCount = 0;


    int masterVolume = 0;
    int frequency = 440;

  

    wxDECLARE_EVENT_TABLE();

};

