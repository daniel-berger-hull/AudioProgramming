#pragma once

#include <wx/wx.h>


#define DEFAULT_MARGIN			 10

#define	LOG_FRAME_WIDTH			 700
#define	LOG_FRAME_HEIGHT		 430
#define	LOG_FRAME_MARGIN		 DEFAULT_MARGIN


#define STATIC_BOX_XPOS			 10
#define STATIC_BOX_YPOS			 10
#define STATIC_BOX_WIDTH		 LOG_FRAME_WIDTH  - ( 4 * LOG_FRAME_MARGIN)
#define STATIC_BOX_HEIGHT		 LOG_FRAME_HEIGHT - (10 * LOG_FRAME_MARGIN)


#define EDIT_BOX_XPOS			 10
#define EDIT_BOX_YPOS			 20
#define EDIT_BOX_WIDTH			 STATIC_BOX_WIDTH  - (2 * LOG_FRAME_MARGIN)
#define EDIT_BOX_HEIGHT			 STATIC_BOX_HEIGHT - (4 * LOG_FRAME_MARGIN)

#define CLEAR_BUTTON_XPOS		 STATIC_BOX_WIDTH - CLEAR_BUTTON_WIDTH
#define CLEAR_BUTTON_YPOS		 STATIC_BOX_HEIGHT + (2 * DEFAULT_MARGIN)
#define CLEAR_BUTTON_WIDTH		 50
#define CLEAR_BUTTON_HEIGHT		 20




class LogFrame : public wxFrame
{

public:
	LogFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void log(const wxString message);
	void log(const wxString message, int value);

	void closeScreen();


private:

	wxPanel* buildMainPanel();
	
	void OnClearLogsButtonClick(wxCommandEvent& event);

	wxTextCtrl* logEditBox;
	wxButton* clearButton;

	wxDECLARE_EVENT_TABLE();
};

