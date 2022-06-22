
#include "MainFrame.h"


#include "EventID.h"
#include "LogFrame.h"
#include "AudioWorkerThread.h"

#include "Tone.h"







wxString CurrentDocPath;


wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)

	EVT_MENU(ID_InitWASAPIButton, MainFrame::OnInitWASAPIClick)
	EVT_MENU(wxID_EXIT, MainFrame::OnExitMenuItemClick)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)

	EVT_BUTTON(ID_PlayButton, MainFrame::OnPlayButtonClick) // Tell the OS to run MainFrame::OnExit when
    EVT_BUTTON(ID_StopButton, MainFrame::OnStopButtonClick) // Tell the OS to run MainFrame::OnExit when

    EVT_RADIOBOX(ID_WaveTypeRadioBox, MainFrame::OnRadioBoxChange)
    EVT_CHECKBOX(wxID_CHECKBOX, MainFrame::OnCheckBoxEvent)
   
    EVT_PAINT(MainFrame::OnPaint)

    EVT_THREAD(ID_AUDIO_WORKER_PLAY_IN_PROGRESS_EVENT_UPDATE, MainFrame::OnAudioWorkerPlayInProgressEventUpdate)
    EVT_THREAD(ID_AUDIO_WORKER_PLAY_COMPLETED_EVENT, MainFrame::OnAudioWorkerPlayCompletedEvent)


wxEND_EVENT_TABLE()



MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    buildMenuBar();
    buildMainPanel();
    SetStatusText("Testing WASAPI!");

    signalType = SINE_SIGNAL;


	startAudioWorkerThread();
}

MainFrame::~MainFrame()
{
    logFrame->closeScreen();

    wxMilliSleep(200);
}



wxMenuBar* MainFrame::buildMenuBar()
{
    wxMenu* menuFile = new wxMenu;


    menuFile->Append(ID_InitWASAPIButton, "&Init\tCtrl-I", "Init WASAPI system");

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);
    CreateStatusBar();

    return menuBar;
}


wxPanel* MainFrame::buildMainPanel()
{
    wxPanel* panel = new wxPanel(this);


    buildVolumeSection(panel);
    buildFrequencySection(panel);
    buildButtonsSection(panel);

	wxArrayString choices;
	choices.Add("Sine");
    choices.Add("Triangle");
    choices.Add("Saw");
	choices.Add("Square");
	choices.Add("Noise");


    m_radioBox = new wxRadioBox(panel, ID_WaveTypeRadioBox,
        "Wave Type", {10,80}, { 350,50 }, 
        choices, 5, wxRA_HORIZONTAL);


    // This should be done in the  wxBEGIN_EVENT_TABLE macro
    // but for some reason, there is a compile error with 
    // the EVT_SLIDER macro...
    Connect(ID_VolumeSlider, wxEVT_COMMAND_SLIDER_UPDATED, wxScrollEventHandler(MainFrame::OnVolumeSliderScroll));
    Connect(ID_FrequencySlider, wxEVT_COMMAND_SLIDER_UPDATED, wxScrollEventHandler(MainFrame::OnFrequencySliderScroll));


    return panel;
}

wxStaticBox* MainFrame::buildVolumeSection(wxPanel* parent)
{
    // Volume Control Section
    wxStaticBox* volumeBox = new wxStaticBox(parent, wxID_ANY, "Volume", { 10,10 }, { 170,60 });
    volumeSlider = new wxSlider(volumeBox, ID_VolumeSlider, 0, 0, 100,
                                wxPoint(5, 20), wxSize(140, -1));

    volumeSlider->SetValue(100);

    volumeText = new wxStaticText(volumeBox, wxID_ANY, "100", wxPoint(145, 25));


    return volumeBox;


}
wxStaticBox* MainFrame::buildFrequencySection(wxPanel* parent)
{
    // Frequency Control Section
    wxStaticBox* frequencyBox = new wxStaticBox(parent, wxID_ANY, "Frequency", { 190,10 }, { 190,60 });

    frequencySlider = new wxSlider(frequencyBox, ID_FrequencySlider, 0, 0, 100,
        wxPoint(5, 20), wxSize(140, -1));

    frequencySlider->SetMin(440);
    frequencySlider->SetMax(7000);

    frequencyText = new wxStaticText(frequencyBox, wxID_ANY, "440", wxPoint(150, 25));

    return frequencyBox;
}

wxStaticBox* MainFrame::buildButtonsSection(wxPanel* parent)
{
    wxStaticBox* buttonsBox = new wxStaticBox(parent, wxID_ANY, " ", { 400,10 }, { 90,90 });


    playButton = new wxButton(buttonsBox, ID_PlayButton, _T("Play"), { 10, 20 }, { 60,25 }, 0);
    stopButton = new wxButton(buttonsBox, ID_StopButton, _T("Stop"), { 10, 50 }, { 60,25 }, 0);
    stopButton->Enable(false);

    return buttonsBox;
}


void MainFrame::startAudioWorkerThread()
{


	audioThread = new AudioWorkerThread(this);
	
	if (audioThread->Create() != wxTHREAD_NO_ERROR)
	{
        log("Can't create thread!");
		return;
	}


	// thread is not running yet, no need for crit sect
	m_audioWorkerThreadRunning = true;

	audioThread->Run();
}



void MainFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{

    wxPaintDC dc(this);

    const wxSize& sz = dc.GetSize();

    //{
    //    // paint the bitmap
    //    wxCriticalSectionLocker locker(m_csBmp);
    //    dc.DrawBitmap(m_bmp, (sz.GetWidth() - GUITHREAD_BMP_SIZE) / 2,
    //        (sz.GetHeight() - GUITHREAD_BMP_SIZE) / 2);
    //}

    //// paint a sort of progress bar with a 10px border:
 /*   dc.SetBrush(*wxRED_BRUSH);
    dc.DrawRectangle(260, 235, 100, 140);*/
}


void MainFrame::OnExitMenuItemClick(wxCommandEvent& event)
{

    audioThread->postCommand(TERMINATE_AUDIO_THREAD_COMMAND);
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a simple test of the Win32 WASAPI Audio system, built using the wxWidgets GUI framework",
        "About RenderSharedWxWidgets", wxOK | wxICON_INFORMATION);
}






void MainFrame::OnInitWASAPIClick(wxCommandEvent& event)
{
	logFrame->log("Init WASAPI Click Handler invoked");	
	audioThread->postCommand(INIT_WASAPI_COMMAND);
}






void MainFrame::OnPlayButtonClick(wxCommandEvent& event)
{
    logFrame->log("Play Click Handler invoked");

    std::list<int> params;
    params.push_back(masterVolume);
    params.push_back(frequency);
    params.push_back(2);
    params.push_back(signalType);

    audioThread->postCommand(PLAY_COMMAND, params);

    m_audioWorkerPlaying = true;
    togglePlayStopButtons(m_audioWorkerPlaying);


}



void MainFrame::OnStopButtonClick(wxCommandEvent& event)
{
    int a = 2;
}





void MainFrame::OnRadioBoxChange(wxCommandEvent& evt)
{   
    wxString text = m_radioBox->GetString(evt.GetSelection());

    signalType = evt.GetSelection() + 1;   // Radio Group indexes start at 0
}


void MainFrame::OnVolumeSliderScroll(wxScrollEvent& event)
{

    masterVolume = volumeSlider->GetValue();
    volumeText->SetLabelText(wxString::Format(wxT("%i"), masterVolume));
}

void MainFrame::OnFrequencySliderScroll(wxScrollEvent& event)
{

    frequency = frequencySlider->GetValue();
    frequencyText->SetLabelText(wxString::Format(wxT("%i"), frequency));
 
}


void MainFrame::OnCheckBoxEvent(wxCommandEvent& evt)
{
    if (evt.IsChecked())
    {
      
        wxLogMessage("Checkbox checked");
    }
    else
    {
        wxLogMessage("Checkbox not checked");
    }
}






void MainFrame::OnAudioWorkerPlayInProgressEventUpdate(wxThreadEvent& event)
{

    //int n = event.GetInt();
    logFrame->log("Play Update ", event.GetInt());
}

void MainFrame::OnAudioWorkerPlayCompletedEvent(wxThreadEvent& event)
{
    m_audioWorkerPlaying = false;
    togglePlayStopButtons(m_audioWorkerPlaying);
    logFrame->log("Play Completed Event received");
}





void MainFrame::log(wxString message)
{
	logFrame->log(message);
}


void MainFrame::log(wxString message, int value)
{

	wxString mystring = wxString::Format(wxT("%i"), value);
	message.append(mystring);
	logFrame->log(message);
}



void MainFrame::togglePlayStopButtons(bool playing)
{
    playButton->Enable(!playing);
    stopButton->Enable(playing);
}