#include "folderTreeStructure.h"

FolderTreeStructureFrame::FolderTreeStructureFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // Create the tree control
    m_treeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);

    // Bind double-click event to the item activated handler
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &FolderTreeStructureFrame::OnItemActivated, this);

    // Add root item
    rootId = m_treeCtrl->AddRoot("Root");

    // get Current Users Home directory
    wxString rootPath = wxGetHomeDir();

    PopulateTree(rootPath, rootId);

    // Set up sizers
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 5);
    SetSizerAndFit(sizer);
}

void FolderTreeStructureFrame::PopulateTree(const wxString &path, const wxTreeItemId &parentItem)
{
    wxDir dir(path);
    if (!dir.IsOpened())
    {
        wxLogError("Failed to open directory: %s", path);
        return;
    }

    wxString filename;
    bool cont = dir.GetFirst(&filename);
    while (cont)
    {
        // Skip hidden folders (starting with a dot)
        if (filename.StartsWith("."))
        {
            cont = dir.GetNext(&filename);
            continue;
        }

        wxString fullPath = wxFileName::DirName(path).GetFullPath() + wxFileName::GetPathSeparator() + filename;

        if (wxFileName::DirExists(fullPath))
        {
            // It's a directory
            wxTreeItemId itemId = m_treeCtrl->AppendItem(parentItem, filename, -1, -1, new FileTreeItemData(fullPath));
            PopulateTree(fullPath, itemId); // Recursively populate subdirectories
        }
        // if you want file data also you can by adding else part.
        cont = dir.GetNext(&filename);
    }
}

void FolderTreeStructureFrame::OnItemActivated(wxTreeEvent &event)
{
    wxTreeItemId itemId = event.GetItem();
    if (m_treeCtrl->ItemHasChildren(itemId))
    {
        m_treeCtrl->Expand(itemId);
    }
    // Retrieve the tree item data
    wxTreeItemData *itemData = m_treeCtrl->GetItemData(itemId);
    if (itemData)
    {
        FileTreeItemData *fileData = dynamic_cast<FileTreeItemData *>(itemData);
        if (fileData)
        {
            // handle the event to display the folder contents in main frame...
            wxFileName fileName = fileData->GetFileName();
            wxLogMessage("Item path: %s", fileName.GetFullPath()); // handle pass
        }
    }
}
