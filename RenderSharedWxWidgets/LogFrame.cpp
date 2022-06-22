#include "LogFrame.h"


#include "EventID.h"



wxBEGIN_EVENT_TABLE(LogFrame, wxFrame)
    EVT_BUTTON(ID_ClearLogButton, LogFrame::OnClearLogsButtonClick) // Tell the OS to run MainFrame::OnExit when
wxEND_EVENT_TABLE()


LogFrame::LogFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    buildMainPanel();
}


wxPanel* LogFrame::buildMainPanel()
{
    wxPanel* panel = new wxPanel(this);


    wxStaticBox* volumeBox = new wxStaticBox(panel, wxID_ANY, "Volume", 
                                            { STATIC_BOX_XPOS,STATIC_BOX_YPOS },
                                            { STATIC_BOX_WIDTH,STATIC_BOX_HEIGHT });

   
     logEditBox = new wxTextCtrl(volumeBox, ID_LogEditBox, " ", 
                                { EDIT_BOX_XPOS,EDIT_BOX_YPOS }, 
                                { EDIT_BOX_WIDTH,EDIT_BOX_HEIGHT },
                         wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr);


    clearButton = new wxButton(panel, ID_ClearLogButton, _T("Clear"),
        { CLEAR_BUTTON_XPOS, CLEAR_BUTTON_YPOS }, { CLEAR_BUTTON_WIDTH ,CLEAR_BUTTON_HEIGHT }, 0);


    return panel;
}


void LogFrame::OnClearLogsButtonClick(wxCommandEvent& event)
{
    logEditBox->SetLabelText("");
}


void LogFrame::log(const wxString message)
{

    wxString currentText =  logEditBox->GetLabelText();

    currentText.append("\n");
    currentText.append(message);
    logEditBox->SetLabelText(currentText);
}

void LogFrame::log(const wxString message, int value)
{
    wxString finalMessage = message;
    wxString valueString = wxString::Format(wxT("%i"), value);
    finalMessage.append(valueString);
    log(finalMessage);
}





void LogFrame::closeScreen()
{
    Close(true);
}

