// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "folderTreeStructure.cpp"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    FolderTreeStructureFrame *leftFolderStructureFrame = new FolderTreeStructureFrame("File Manager", wxPoint(50, 50), wxSize(800, 600));
    leftFolderStructureFrame->Show(true);
    return true;
}
