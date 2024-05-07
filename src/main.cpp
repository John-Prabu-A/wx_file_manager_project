#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/grid.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <vector>
#include <wx/splitter.h>
#include <wx/wrapsizer.h>
#include <string>
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include "bufferedbitmap.h"
#include "FolderIcon.cpp"

class BufferedBitmap;
class FolderIcon;

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class FileTreeItemData : public wxTreeItemData
{
public:
    FileTreeItemData(const wxFileName &fileName) : m_fileName(fileName) {}
    const wxFileName &GetFileName() const { return m_fileName; }

private:
    wxFileName m_fileName;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size, const wxString &path = "");
    void SetScaledImage(const wxImage &image, bool isDefaultPath = false);
    void OnResize(wxSizeEvent &event);
    void PopulateTree(const wxString &path, const wxTreeItemId &parentItem);
    void PopulateFolderIcons(const wxString &path, wxSizer *sizer);
    void OnItemActivated(wxTreeEvent &event);

private:
    wxDECLARE_EVENT_TABLE();

    wxGridBagSizer *sizer;
    BufferedBitmap *bitmap;
    wxStaticText *dimensionsValue;
    wxGridBagSizer *propertiesSizer;
    wxGridBagSizer *top_grid;
    wxBoxSizer *quickAccessSizer;
    wxBoxSizer *folderStructureSizer;
    wxString path;
    wxSize previousSize;
    bool isMaximized;
    bool isRestored;
    FolderIcon *folderIcon;
    std::string currentPath;
    wxTreeCtrl *m_treeCtrl;
    wxTreeItemId rootId;
    wxScrolledWindow *panel_top;
    wxScrolledWindow *panel_bottom;
    wxScrolledWindow *panel_left;
    wxScrolled<wxPanel> *panel_mid;
    wxScrolledWindow *panel_right;
    wxTextCtrl *nameText;
    int margin;
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnResize)
        EVT_TREE_ITEM_ACTIVATED(wxID_ANY, MyFrame::OnItemActivated)
            wxEND_EVENT_TABLE()

                wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("File Explorer", wxDefaultPosition, wxDefaultSize);
    wxIcon icon;
    wxFileName iconPath("src/resource/app_logo.ico");
    icon.LoadFile(iconPath.GetFullPath(), wxBITMAP_TYPE_ICO);
    frame->SetIcon(icon);
    frame->Maximize();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size, const wxString &p)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), sizer(nullptr), path(p), previousSize(size), isMaximized(false), isRestored(false), bitmap(nullptr), dimensionsValue(nullptr), propertiesSizer(nullptr), top_grid(nullptr), quickAccessSizer(nullptr), folderStructureSizer(nullptr), folderIcon(nullptr)
{
    m_treeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    rootId = m_treeCtrl->AddRoot("Root");
    wxString rootPath = wxGetHomeDir();
    PopulateTree(rootPath, rootId);
    currentPath = wxGetHomeDir().ToStdString();
    wxSize screenSize = wxGetDisplaySize();
    SetMinSize(wxSize(600, 600));
    margin = 10;

    panel_top = new wxScrolledWindow(this, wxID_ANY);
    panel_bottom = new wxScrolledWindow(this, wxID_ANY);
    panel_left = new wxScrolledWindow(panel_bottom, wxID_ANY);
    panel_mid = new wxScrolled<wxPanel>(panel_bottom, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    panel_right = new wxScrolledWindow(panel_bottom, wxID_ANY);

    // Set scroll rates
    panel_top->SetScrollRate(10, 10);
    panel_bottom->SetScrollRate(10, 10);
    panel_left->SetScrollRate(10, 10);
    panel_mid->SetScrollRate(10, 10);
    panel_right->SetScrollRate(10, 10);

    panel_top->SetMinSize(wxSize(-1, 35));
    panel_left->SetMinSize(wxSize(100, 300));
    panel_mid->SetMinSize(wxSize(300, 300));
    panel_right->SetMinSize(wxSize(300, 300));

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(panel_top, 0, wxEXPAND);
    sizer->Add(panel_bottom, 1, wxEXPAND);

    wxSizer *sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_bottom->Add(panel_left, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    sizer_bottom->Add(panel_mid, 1, wxEXPAND);
    sizer_bottom->Add(panel_right, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    panel_bottom->SetSizerAndFit(sizer_bottom);

    top_grid = new wxGridBagSizer(margin, margin);
    const int iconSize = 32;
    wxButton *backButton = new wxButton(panel_top, wxID_ANY, wxT(" \u2190 "), wxDefaultPosition, wxSize(-1, iconSize));
    wxButton *forwardButton = new wxButton(panel_top, wxID_ANY, wxT(" \u2192 "), wxDefaultPosition, wxSize(-1, iconSize));
    wxButton *reloadButton = new wxButton(panel_top, wxID_ANY, wxT(" \u21BB "), wxDefaultPosition, wxSize(-1, iconSize));

    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(backButton, 0, wxLEFT, margin);
    buttonSizer->Add(forwardButton, 0, wxLEFT, margin);
    buttonSizer->Add(reloadButton, 0, wxLEFT, margin);

    nameText = new wxTextCtrl(panel_top, wxID_ANY);
    nameText->SetValue(currentPath);
    nameText->SetEditable(false);

    auto nameFormSizer = new wxBoxSizer(wxHORIZONTAL);
    nameFormSizer->Add(nameText, 1, wxEXPAND | wxRIGHT, margin);

    wxSearchCtrl *searchBar = new wxSearchCtrl(panel_top, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, iconSize), wxTE_PROCESS_ENTER);
    searchBar->ShowSearchButton(true);
    searchBar->SetDescriptiveText("Search");
    searchBar->SetMinSize(wxSize(100, iconSize));
    nameFormSizer->Add(searchBar, 0, wxRIGHT, margin);

    top_grid->Add(buttonSizer, wxGBPosition(0, 0), wxGBSpan(1, 1), wxEXPAND | wxALIGN_LEFT);
    top_grid->Add(nameFormSizer, wxGBPosition(0, 1), wxGBSpan(1, 2), wxEXPAND | wxALIGN_CENTER_VERTICAL);
    top_grid->AddGrowableCol(1);

    const int IMAGE_SIZE = 300;
    bitmap = new BufferedBitmap(panel_right, wxID_ANY, wxBitmap(wxSize(IMAGE_SIZE, IMAGE_SIZE)), wxDefaultPosition, wxSize(IMAGE_SIZE, IMAGE_SIZE));
    wxFileName imagePath("src/resource/app_logo.png");
    path = imagePath.GetFullPath();
    wxImage image;
    image.LoadFile(path);
    SetScaledImage(image, true);

    auto kindLabel = new wxStaticText(panel_right, wxID_ANY, "");
    auto sizeLabel = new wxStaticText(panel_right, wxID_ANY, "");
    auto dimensionsLabel = new wxStaticText(panel_right, wxID_ANY, "");

    auto kindValue = new wxStaticText(panel_right, wxID_ANY, "");
    auto sizeValue = new wxStaticText(panel_right, wxID_ANY, "");
    dimensionsValue = new wxStaticText(panel_right, wxID_ANY, "");

    propertiesSizer = new wxGridBagSizer(margin, margin);
    propertiesSizer->Add(bitmap, {0, 0}, {1, 2}, wxALIGN_CENTER | wxALL, margin);
    propertiesSizer->Add(kindLabel, {1, 0}, {1, 1}, wxALIGN_RIGHT | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->Add(sizeLabel, {2, 0}, {1, 1}, wxALIGN_RIGHT | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->Add(dimensionsLabel, {3, 0}, {1, 1}, wxALIGN_RIGHT | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->Add(kindValue, {1, 1}, {1, 1}, wxEXPAND | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->Add(sizeValue, {2, 1}, {1, 1}, wxEXPAND | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->Add(dimensionsValue, {3, 1}, {1, 1}, wxEXPAND | wxLEFT | wxBOTTOM, margin);
    propertiesSizer->AddGrowableRow(0);

    quickAccessSizer = new wxBoxSizer(wxVERTICAL);

    folderStructureSizer = new wxWrapSizer(wxHORIZONTAL);
    // Populate FolderIcons
    PopulateFolderIcons(rootPath, folderStructureSizer);

    this->SetSizerAndFit(sizer);
    panel_top->SetSizer(top_grid);
    panel_right->SetSizer(propertiesSizer);
    panel_left->SetSizer(quickAccessSizer);
    panel_mid->SetSizer(folderStructureSizer);

    wxInitAllImageHandlers();
}

void MyFrame::SetScaledImage(const wxImage &image, bool isDefaultPath)
{
    if (!propertiesSizer || !bitmap || !dimensionsValue)
        return;

    wxSizerItem *bitmapItem = propertiesSizer->GetItem(bitmap);
    if (bitmapItem)
    {
        wxSize bitmapSize = bitmapItem->GetSize();
        int bitmapWidth = bitmapSize.GetWidth();
        int bitmapHeight = bitmapSize.GetHeight();

        double widthScale = static_cast<double>(bitmapWidth) * 0.90 / image.GetWidth();
        double heightScale = static_cast<double>(bitmapHeight) * 0.90 / image.GetHeight();
        double scale = std::min(widthScale, heightScale);

        wxImage scaledImage = image.Scale(image.GetWidth() * scale, image.GetHeight() * scale);

        bitmap->SetBitmap(wxBitmap(scaledImage));
        if (!isDefaultPath)
            dimensionsValue->SetLabel(wxString::Format("%d x %d", scaledImage.GetWidth(), scaledImage.GetHeight()));
        propertiesSizer->Layout();
    }
}

void MyFrame::OnResize(wxSizeEvent &event)
{
    if (path.empty())
    {
        event.Skip();
        return;
    }

    event.Skip();

    if (!bitmap)
        return;

    if (isMaximized || isRestored)
    {
        isMaximized = false;
        isRestored = false;

        wxImage image;
        if (!image.LoadFile(path))
        {
            wxMessageBox("Failed to load image");
            return;
        }

        SetScaledImage(image);
        this->Layout();
    }
    else
    {
        wxSize currentSize = GetClientSize();

        if (currentSize != previousSize)
        {
            wxImage image;
            if (!image.LoadFile(path))
            {
                wxMessageBox("Failed to load image");
                return;
            }
            SetScaledImage(image);
            this->Layout();
        }

        previousSize = currentSize;
    }
}

void MyFrame::PopulateTree(const wxString &path, const wxTreeItemId &parentItem)
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
            // Add an empty child item to indicate it has not been populated yet
            m_treeCtrl->AppendItem(itemId, "");
        }
        // if you want file data also you can by adding else part.
        cont = dir.GetNext(&filename);
    }
}

void MyFrame::OnItemActivated(wxTreeEvent &event)
{
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk())
        return;

    // Clear existing icons
    folderStructureSizer->Clear(true);

    // Get the path associated with the activated item
    FileTreeItemData *itemData = dynamic_cast<FileTreeItemData *>(m_treeCtrl->GetItemData(itemId));
    if (!itemData)
        return;

    wxString folderPath = itemData->GetFileName().GetFullPath();

    // Populate subfolder icons
    PopulateFolderIcons(folderPath, folderStructureSizer);
}

void MyFrame::PopulateFolderIcons(const wxString &path, wxSizer *sizer)
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
            std::string folderNameStr = filename.ToStdString();
            FolderIcon *folderIcon = new FolderIcon(panel_mid, folderNameStr, fullPath, wxSize(70, 70));
            sizer->Add(folderIcon, 0, wxALL, margin);

            folderIcon->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent &event)
                             {
                wxString folderPath = folderIcon->GetFolderPath();
                // Clear existing icons
                nameText->SetValue(folderPath);
                sizer->Clear(true);
                // Populate subfolder icons
                PopulateFolderIcons(folderPath, sizer); });
        }
        // if you want file data also you can by adding else part.
        cont = dir.GetNext(&filename);
    }
}
