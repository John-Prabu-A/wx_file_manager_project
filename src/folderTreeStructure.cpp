#pragma once

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
FolderTreeStructurePanel::FolderTreeStructurePanel(wxWindow *parent, const wxSize &size, wxWindowID id, const wxPoint &pos, long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name)
{
    // Create the tree control
    m_treeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);

    // Bind double-click event to the item activated handler
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &FolderTreeStructurePanel::OnItemActivated, this);

    // Add root item
    rootId = m_treeCtrl->AddRoot("Root");

    // Get current user's home directory
    wxString rootPath = wxGetHomeDir();

    PopulateTree(rootPath, rootId);

    // Set up sizers
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 5);
    SetSizerAndFit(sizer);
}

void FolderTreeStructurePanel::PopulateTree(const wxString &path, const wxTreeItemId &parentItem)
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

void FolderTreeStructurePanel::OnItemActivated(wxTreeEvent &event)
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
            // Handle the event to display the folder contents in the main frame...
            wxFileName fileName = fileData->GetFileName();
            wxLogMessage("Item path: %s", fileName.GetFullPath()); // Handle pass
        }
    }
}
