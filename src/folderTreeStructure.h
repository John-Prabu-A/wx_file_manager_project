#pragma once

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>

class MyFrame;

class FileTreeItemData : public wxTreeItemData
{
public:
    FileTreeItemData(const wxString &filePath) : m_filePath(filePath) {}
    const wxString &GetFilePath() const { return m_filePath; }

private:
    wxString m_filePath;
};

class FolderTreeStructurePanel : public wxPanel
{
public:
    FolderTreeStructurePanel(MyFrame *parentFrame, wxWindow *parent, const wxSize &size = wxDefaultSize, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr);

private:
    void PopulateTree(const wxString &path, const wxTreeItemId &parentItem);
    void OnItemActivated(wxTreeEvent &event);
    void OnLeftDown(wxMouseEvent &event);

    wxTreeCtrl *m_treeCtrl;
    wxTreeItemId rootId;
    MyFrame *parentFrame;

    wxDECLARE_EVENT_TABLE();
};
