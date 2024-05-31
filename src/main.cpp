#include "MyFrame.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    // EVT_SIZE(MyFrame::OnResize)
    wxEND_EVENT_TABLE()

        wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("File Explorer", wxDefaultPosition, wxDefaultSize);
    // frame->Bind(EVT_SIZE, OnResize);
    // frame->Bind(EVT_TREE_ITEM_ACTIVATED, OnItemActivated);
    wxIcon icon;
    wxFileName iconPath("src/resource/app_logo.ico");
    icon.LoadFile(iconPath.GetFullPath(), wxBITMAP_TYPE_ICO);
    frame->SetIcon(icon);
    frame->Maximize();
    frame->Show(true);
    return true;
}
