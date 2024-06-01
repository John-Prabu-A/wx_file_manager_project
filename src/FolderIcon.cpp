#include "FolderIcon.h"
#include "FileIcon.h"

FolderIcon::FolderIcon(wxWindow *parent, std::string folderName, wxString folderPath, const wxSize &size, wxWindowID id,
                       const wxPoint &pos, long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name),
      m_folderName(folderName),
      m_folderPath(folderPath),
      m_active(false)
{
    // Load icons
    m_iconNormal = wxIcon("src/resource/folder_icon_normal.ico", wxBITMAP_TYPE_ICO);
    m_iconActive = wxIcon("src/resource/folder_icon_active.ico", wxBITMAP_TYPE_ICO);

    // Display folder icon
    m_iconBitmap = new wxStaticBitmap(this, wxID_ANY, m_iconNormal);

    // add space in the folder name if more than 12 characters without space
    wxString newFolderName = "";
    if (m_folderName.length() > 11)
    {
        int j = 0;
        int count = 0;
        for (int i = 0; i < m_folderName.length(); i++)
        {
            if (j > 0 && j % 11 == 0)
            {
                count++;
                if (count == 2)
                {
                    newFolderName += "...";
                    break;
                }
                newFolderName += " ";
            }
            newFolderName += m_folderName[i];
            j++;
        }
    }
    else
    {
        newFolderName = m_folderName;
    }

    // Display folder name
    m_text = new wxStaticText(this, wxID_ANY, newFolderName, wxDefaultPosition, wxSize(FromDIP(100), FromDIP(40)), wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE);
    m_text->Wrap(FromDIP(100));
    // Sizers to layout the components
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddStretchSpacer();
    sizer->Add(m_iconBitmap, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
    sizer->Add(m_text, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
    sizer->AddStretchSpacer();
    SetSizer(sizer);

    // Set fixed size for the sizer
    SetMinSize(wxSize(FromDIP(100), FromDIP(120)));
    SetMaxSize(wxSize(FromDIP(100), FromDIP(120)));

    // Binding events
    parent->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnLeftClick, this);
    m_iconBitmap->Bind(wxEVT_LEFT_DCLICK, &FolderIcon::OnDoubleClick, this);
    m_iconBitmap->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnIconClick, this);
    m_text->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnTextClick, this);
    m_text->Bind(wxEVT_LEFT_DCLICK, &FolderIcon::OnTextDoubleClick, this);
}

void FolderIcon::SetIconOpacity(unsigned char alpha)
{
    wxBitmap bmp = m_iconBitmap->GetBitmap();
    wxImage img = bmp.ConvertToImage();
    if (!img.HasAlpha())
    {
        img.InitAlpha();
    }
    unsigned char *alphaData = img.GetAlpha();
    if (!alphaData)
    {
        alphaData = new unsigned char[img.GetWidth() * img.GetHeight()];
        memset(alphaData, alpha, img.GetWidth() * img.GetHeight());
        img.SetAlpha(alphaData, true);
    }
    else
    {
        for (int y = 0; y < img.GetHeight(); ++y)
        {
            for (int x = 0; x < img.GetWidth(); ++x)
            {
                alphaData[y * img.GetWidth() + x] = alpha;
            }
        }
    }
    m_iconBitmap->SetBitmap(wxBitmap(img));
    Refresh();
}

bool FolderIcon::isActive()
{
    return FolderIcon::m_active;
}

void FolderIcon::Activate()
{
    FolderIcon::m_active = true;
    this->SetBackgroundColour(wxColour(54, 192, 255, 100));
    m_iconBitmap->SetIcon(m_iconActive);
}

void FolderIcon::Deactivate()
{
    FolderIcon::m_active = false;
    this->SetBackgroundColour(wxNullColour);
    m_iconBitmap->SetIcon(m_iconNormal);
}

wxString FolderIcon::GetFolderPath() const
{
    return m_folderPath;
}

void FolderIcon::SetFolderPath(wxString folderPath)
{
    m_folderPath = folderPath;
}

void FolderIcon::SetFolderName(wxString folderName)
{
    m_folderName = folderName;
}

wxString FolderIcon::GetFolderName() const
{
    return m_folderName;
}

void FolderIcon::OnLeftClick(wxMouseEvent &event)
{
    wxWindowList &children = GetParent()->GetChildren();
    for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
    {
        FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
        if (folderIcon)
        {
            folderIcon->Deactivate();
        }
        FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
        if (fileIcon)
        {
            fileIcon->Deactivate();
        }
    }

    event.Skip();
}

void FolderIcon::OnIconClick(wxMouseEvent &event)
{
    if (!FolderIcon::m_active)
    {
        wxWindowList &children = GetParent()->GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
        {
            FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
            if (folderIcon)
            {
                folderIcon->Deactivate();
            }
            FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
            if (fileIcon)
            {
                fileIcon->Deactivate();
            }
        }
        Activate();
        wxCommandEvent dummyEvent;
        wxPostEvent(GetParent(), dummyEvent);
    }
}

void FolderIcon::OnTextClick(wxMouseEvent &event)
{
    if (!FolderIcon::m_active)
    {
        wxWindowList &children = GetParent()->GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
        {
            FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
            if (folderIcon)
            {
                folderIcon->Deactivate();
            }
            FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
            if (fileIcon)
            {
                fileIcon->Deactivate();
            }
        }
        Activate();
        wxCommandEvent dummyEvent;
        wxPostEvent(GetParent(), dummyEvent);
    }
    event.Skip();
}

void FolderIcon::OnDoubleClick(wxMouseEvent &event)
{
    wxCommandEvent folderEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
    folderEvent.SetString(m_folderPath);
    GetEventHandler()->ProcessEvent(folderEvent);
}

void FolderIcon::OnTextDoubleClick(wxMouseEvent &event)
{
    wxCommandEvent folderEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
    folderEvent.SetString(m_folderPath);
    GetEventHandler()->ProcessEvent(folderEvent);
}
