#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>

class FolderTreeStructureFrame : public wxFrame
{
public:
    FolderTreeStructureFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    void PopulateTree(const wxString &path, const wxTreeItemId &parentItem);
    void OnItemActivated(wxTreeEvent &event);
    wxString GetUserHomePath();
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
