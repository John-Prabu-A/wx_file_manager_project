#pragma once

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>

class FolderTreeStructurePanel : public wxPanel
{
public:
    FolderTreeStructurePanel(wxWindow *parent, const wxSize &size = wxDefaultSize, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr);

private:
    void PopulateTree(const wxString &path, const wxTreeItemId &parentItem);
    void OnItemActivated(wxTreeEvent &event);

    wxTreeCtrl *m_treeCtrl;
    wxTreeItemId rootId;
};

class FileTreeItemData : public wxTreeItemData
{
public:
    FileTreeItemData(const wxFileName &fileName) : m_fileName(fileName) {}
    const wxFileName &GetFileName() const { return m_fileName; }

private:
    wxFileName m_fileName;
};

#include "folderTreeStructure.cpp"