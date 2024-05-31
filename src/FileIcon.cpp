#include "FileIcon.h"
#include "FolderIcon.h"

std::map<wxString, wxString> FileIcon::fileIconMap;
FileIcon::FileIcon(wxWindow *parent, wxString fileName, wxString filePath, const wxSize &size, wxWindowID id,
                   const wxPoint &pos, long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name),
      m_fileName(fileName),
      m_filePath(filePath),
      m_active(false)
{
    // Initialize file icon map if not already initialized
    if (fileIconMap.empty())
    {
        InitializeFileIconMap();
    }
    wxString fileExtension = GetFileExtension(filePath);
    wxString iconPath = GetIconPath(fileExtension);
    m_iconBitmap = new wxStaticBitmap(this, wxID_ANY, wxIcon(iconPath, wxBITMAP_TYPE_ICO));

    // add space in the file name if more than 12 characters without space
    wxString newFileName = "";
    // remove extension from m_fileName
    wxString noExtFileName = m_fileName.substr(0, m_fileName.find_last_of("."));

    if (noExtFileName.length() > 11)
    {
        bool isAddSpace = true;
        for (int i = 0; i < noExtFileName.length(); i++)
        {
            if (i > 0 && i < noExtFileName.length() - 1)
            {
                if (noExtFileName[i] == ' ' || noExtFileName[i - 1] == ' ' || noExtFileName[i + 1] == ' ')
                {
                    isAddSpace = false;
                }
            }
            if (i % 11 == 0 && i != 0)
            {
                if (isAddSpace)
                {
                    newFileName += " ";
                    isAddSpace = true;
                }
            }
            newFileName += noExtFileName[i];
        }
    }
    else
    {
        newFileName = noExtFileName;
    }
    if (newFileName.length() > 23)
    {
        newFileName = newFileName.substr(0, 20);
        newFileName += "...";
    }

    m_text = new wxStaticText(this, wxID_ANY, newFileName, wxDefaultPosition, wxSize(FromDIP(100), FromDIP(40)), wxALIGN_CENTER_HORIZONTAL | wxST_NO_AUTORESIZE);
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
    parent->Bind(wxEVT_LEFT_DOWN, &FileIcon::OnLeftClick, this);
    m_iconBitmap->Bind(wxEVT_LEFT_DCLICK, &FileIcon::OnDoubleClick, this);
    m_iconBitmap->Bind(wxEVT_LEFT_DOWN, &FileIcon::OnIconClick, this);
    m_text->Bind(wxEVT_LEFT_DOWN, &FileIcon::OnTextClick, this);
    m_text->Bind(wxEVT_LEFT_DCLICK, &FileIcon::OnTextDoubleClick, this);
}

void FileIcon::SetIconOpacity(unsigned char alpha)
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

void FileIcon::Activate()
{
    FileIcon::m_active = true;
    this->SetBackgroundColour(wxColour(54, 192, 255, 100));
}

void FileIcon::Deactivate()
{
    FileIcon::m_active = false;
    this->SetBackgroundColour(wxNullColour);
}

bool FileIcon::isActive()
{
    return FileIcon::m_active;
}

wxString FileIcon::GetFilePath() const
{
    return m_filePath;
}

wxString FileIcon::GetFileName() const
{
    return FileIcon::m_fileName;
}

void FileIcon::SetFileName(wxString fileName)
{
    FileIcon::m_fileName = fileName;
}

void FileIcon::SetFilePath(wxString filePath)
{
    m_filePath = filePath;
}

void FileIcon::OnLeftClick(wxMouseEvent &event)
{
    wxWindowList &children = GetParent()->GetChildren();
    for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
    {
        FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
        if (fileIcon)
        {
            fileIcon->Deactivate();
        }
        FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
        if (folderIcon)
        {
            folderIcon->Deactivate();
        }
    }
    event.Skip();
}

void FileIcon::OnIconClick(wxMouseEvent &event)
{
    if (!FileIcon::m_active)
    {
        wxWindowList &children = GetParent()->GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
        {
            FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
            if (fileIcon)
            {
                fileIcon->Deactivate();
            }
            FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
            if (folderIcon)
            {
                folderIcon->Deactivate();
            }
        }
        Activate();
    }
    std::cout << "Icon-clicked on file: " << m_fileName << std::endl;
    event.Skip();
}

void FileIcon::OnTextClick(wxMouseEvent &event)
{
    if (!FileIcon::m_active)
    {
        wxWindowList &children = GetParent()->GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
        {
            FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it);
            if (fileIcon)
            {
                fileIcon->Deactivate();
            }
            FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
            if (folderIcon)
            {
                folderIcon->Deactivate();
            }
        }
        Activate();
    }
    std::cout << "Text-clicked on file: " << m_fileName << std::endl;
    event.Skip();
}

void FileIcon::OnDoubleClick(wxMouseEvent &event)
{
    wxCommandEvent fileEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
    fileEvent.SetString(m_filePath);
    GetEventHandler()->ProcessEvent(fileEvent);
    std::cout << "Double-clicked on file: " << m_fileName << std::endl;
    event.Skip();
}

void FileIcon::OnTextDoubleClick(wxMouseEvent &event)
{
    wxCommandEvent fileEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
    fileEvent.SetString(m_filePath);
    GetEventHandler()->ProcessEvent(fileEvent);
    std::cout << "Text-double-clicked on file: " << m_fileName << std::endl;
    event.Skip();
}

void FileIcon::InitializeFileIconMap()
{
    // Add mappings for file extensions and corresponding icon paths
    fileIconMap["txt"] = "src/resource/text_icon.png";
    // fileIconMap["doc"] = "src/resource/word_icon.ico";
    // fileIconMap["pdf"] = "src/resource/pdf_icon.ico";
    // fileIconMap["jpg"] = "src/resource/image_icon.ico";
    // fileIconMap["png"] = "src/resource/image_icon.ico";
    // fileIconMap["xlsx"] = "src/resource/excel_icon.ico";
    // fileIconMap["pptx"] = "src/resource/powerpoint_icon.ico";
}

wxString FileIcon::GetFileExtension(wxString filePath)
{
    wxFileName fn(filePath);
    return fn.GetExt();
}

wxString FileIcon::GetIconPath(wxString fileExtension)
{
    std::cout << "File extension: " << fileExtension << std::endl;
    // Lookup icon path from file extension
    if (fileIconMap.find(fileExtension) != fileIconMap.end())
    {
        return fileIconMap[fileExtension];
    }
    // Default icon path if extension not found
    return "src/resource/default_icon.png";
}
