#include "folderTreeStructure.h"
#include "MyFrame.h"

wxBEGIN_EVENT_TABLE(FolderTreeStructurePanel, wxPanel)
    EVT_TREE_ITEM_ACTIVATED(wxID_ANY, FolderTreeStructurePanel::OnItemActivated)
        wxEND_EVENT_TABLE()

            FolderTreeStructurePanel::FolderTreeStructurePanel(MyFrame *frame, wxWindow *parent, const wxSize &size, wxWindowID id, const wxPoint &pos, long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name), parentFrame(frame)
{
    // Create the tree control
    m_treeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);

    // Add root item
    rootId = m_treeCtrl->AddRoot("Root");

    // Get current user's home directory
    wxString rootPath = wxGetHomeDir();

    PopulateTree(rootPath, rootId);

    // Set up sizers
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 0);
    wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_treeCtrl->SetFont(font);
    SetSizerAndFit(sizer);

    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &FolderTreeStructurePanel::OnItemActivated, this);
    m_treeCtrl->Bind(wxEVT_LEFT_DOWN, &FolderTreeStructurePanel::OnLeftDown, this);
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

        wxString fullPath = path + wxFileName::GetPathSeparator() + filename;

        if (wxFileName::DirExists(fullPath))
        {
            // It's a directory
            wxTreeItemId itemId = m_treeCtrl->AppendItem(parentItem, filename, -1, -1, new FileTreeItemData(fullPath));
            PopulateTree(fullPath, itemId); // Recursively populate subdirectories
        }
        // If you want file data also you can by adding else part.
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
            if (!parentFrame->backwardStack.empty())
            {
                if (parentFrame->backwardStack.top() != parentFrame->currentPath)
                    parentFrame->backwardStack.push(parentFrame->currentPath);
            }
            else
                parentFrame->backwardStack.push(parentFrame->currentPath);
            parentFrame->currentPath = fileData->GetFilePath();
            parentFrame->OnFolderPathChange(parentFrame->currentPath);
        }
    }
}

void FolderTreeStructurePanel::OnLeftDown(wxMouseEvent &event)
{
    // Check if any item is selected in the tree control
    wxTreeItemId selection = m_treeCtrl->GetSelection();
    if (selection.IsOk())
    {
        // Deselect the selected item
        m_treeCtrl->Unselect();
    }
    event.Skip();
}
