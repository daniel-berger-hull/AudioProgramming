// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif


#include <iostream>

#include <wx/wx.h>

#include "MainFrame.h"
#include "LogFrame.h"

    
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};


wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{



    MainFrame* frame = new MainFrame("Hello World", wxPoint(50, 50), wxSize(800, 230));
    frame->Show(true);

    LogFrame* logFrame = new LogFrame("Logs", wxPoint(900, 50), wxSize(LOG_FRAME_WIDTH, LOG_FRAME_HEIGHT));
    logFrame->Show(true);



    frame->setLogFrame(logFrame);

    return true;
}
