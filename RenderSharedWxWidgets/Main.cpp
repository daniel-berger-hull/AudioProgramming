// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif




#include <wx/wx.h>

#include "MainFrame.h"

    
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

    //frame->SetBackgroundColour(wxStockGDI::BRUSH_BLUE);
    return true;
}
