#include "MyFrame.h"

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size, const wxString &p)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), sizer(nullptr), path(p), previousSize(size),
      isMaximized(false), isRestored(false), bitmap(nullptr), dimensionsValue(nullptr),
      propertiesSizer(nullptr), top_grid(nullptr), quickAccessSizer(nullptr),
      folderStructureSizer(nullptr), folderIcon(nullptr)
{
    cutBuffer = "";
    copyBuffer = "";
    isCut = false;
    wxString rootPath = wxGetHomeDir();
    currentPath = wxGetHomeDir();
    wxSize screenSize = wxGetDisplaySize();
    SetMinSize(wxSize(600, 600));

    panel_top = new wxScrolledWindow(this, wxID_ANY);
    panel_bottom = new wxScrolledWindow(this, wxID_ANY);
    panel_left = new wxScrolledWindow(panel_bottom, wxID_ANY);
    panel_mid = new wxScrolled<wxPanel>(panel_bottom, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    panel_right = new wxScrolledWindow(panel_bottom, wxID_ANY);

    // context menu
    panel_mid->Bind(wxEVT_CONTEXT_MENU, &MyFrame::OnShowContextMenu, this);

    // Set scroll rates
    panel_top->SetScrollRate(10, 10);
    panel_bottom->SetScrollRate(10, 10);
    panel_left->SetScrollRate(10, 10);
    panel_mid->SetScrollRate(10, 10);
    panel_right->SetScrollRate(10, 10);

    panel_top->SetMinSize(wxSize(-1, 50));
    panel_left->SetMinSize(wxSize(300, 300));
    panel_mid->SetMinSize(wxSize(300, 300));
    panel_right->SetMinSize(wxSize(300, 300));

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(panel_top, 0, wxEXPAND);
    mainSizer->Add(panel_bottom, 1, wxEXPAND);

    wxBoxSizer *sizer_bottom = new wxBoxSizer(wxHORIZONTAL);
    sizer_bottom->Add(panel_left, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    sizer_bottom->Add(panel_mid, 1, wxEXPAND);
    sizer_bottom->Add(panel_right, 0, wxEXPAND | wxLEFT | wxRIGHT, margin);
    panel_bottom->SetSizerAndFit(sizer_bottom);

    top_grid = new wxGridBagSizer(margin, margin);
    const int iconSize = FromDIP(32);
    StyledButton *gotoParentButton = new StyledButton(panel_top, wxID_ANY, wxT("\u2191"), wxDefaultPosition, wxDefaultSize);
    StyledButton *backButton = new StyledButton(panel_top, wxID_ANY, wxT("\u2190"), wxDefaultPosition, wxDefaultSize);
    StyledButton *forwardButton = new StyledButton(panel_top, wxID_ANY, wxT("\u2192"), wxDefaultPosition, wxDefaultSize);
    StyledButton *reloadButton = new StyledButton(panel_top, wxID_ANY, wxT("\u21BB"), wxDefaultPosition, wxDefaultSize);

    // handle navigation button events
    gotoParentButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                           {
        gotoParentDirectory();
        OnFolderPathChange(MyFrame::currentPath); });

    backButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                     {
        navigateBack();
        OnFolderPathChange(MyFrame::currentPath); });

    forwardButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                        {
        navigateForward();
        OnFolderPathChange(MyFrame::currentPath); });

    reloadButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                       { OnFolderPathChange(MyFrame::currentPath); });

    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(gotoParentButton, 0, wxLEFT | wxTOP, margin);
    buttonSizer->Add(backButton, 0, wxLEFT | wxTOP, margin);
    buttonSizer->Add(forwardButton, 0, wxLEFT | wxTOP, margin);
    buttonSizer->Add(reloadButton, 0, wxLEFT | wxTOP, margin);

    nameText = new wxTextCtrl(panel_top, wxID_ANY);
    nameText->SetEditable(false);

    wxBoxSizer *nameFormSizer = new wxBoxSizer(wxHORIZONTAL);
    nameFormSizer->Add(nameText, 3, wxEXPAND | wxLEFT | wxTOP, margin);

    searchBar = new wxSearchCtrl(panel_top, wxID_ANY);
    searchBar->Bind(wxEVT_TEXT, &MyFrame::OnSearch, this);
    searchButton = new StyledButton(panel_top, wxID_ANY, wxT(" Search "), wxDefaultPosition, wxDefaultSize);
    searchButton->Bind(wxEVT_BUTTON, &MyFrame::OnSearchButton, this);
    searchBar->ShowSearchButton(true);
    searchBar->SetDescriptiveText("Search");
    searchBar->SetMinSize(wxSize(100, iconSize));

    searchResultList = new wxDataViewListCtrl(this, wxID_ANY);
    searchResultColumn = searchResultList->AppendTextColumn("Results");

    nameFormSizer->Add(searchBar, 1, wxLEFT | wxTOP, margin);
    nameFormSizer->Add(searchButton, 0, wxRIGHT | wxLEFT | wxTOP, margin);

    top_grid->Add(buttonSizer, wxGBPosition(0, 0), wxGBSpan(1, 1), wxEXPAND | wxALIGN_LEFT);
    top_grid->Add(nameFormSizer, wxGBPosition(0, 1), wxGBSpan(1, 2), wxEXPAND | wxALIGN_CENTER_VERTICAL);
    top_grid->AddGrowableCol(1);

    // Initialize UI controls
    nameValue = new wxStaticText(panel_right, wxID_ANY, "", wxPoint(10, 10));
    sizeValue = new wxStaticText(panel_right, wxID_ANY, "", wxPoint(10, 30));
    lastModifiedValue = new wxStaticText(panel_right, wxID_ANY, "", wxPoint(10, 50));
    permissionsValue = new wxStaticText(panel_right, wxID_ANY, "", wxPoint(10, 70));

    nameLabel = new wxStaticText(panel_right, wxID_ANY, "Name:", wxPoint(10, 10));
    sizeLabel = new wxStaticText(panel_right, wxID_ANY, "Size:", wxPoint(10, 30));
    lastModifiedLabel = new wxStaticText(panel_right, wxID_ANY, "Last Modified:", wxPoint(10, 50));
    permissionsLabel = new wxStaticText(panel_right, wxID_ANY, "Permissions:", wxPoint(10, 70));

    // wxStaticText *kindLabel = new wxStaticText(panel_right, wxID_ANY, "Kind:");
    // wxStaticText *kindValue = new wxStaticText(panel_right, wxID_ANY, "");
    // wxStaticText *sizeLabel = new wxStaticText(panel_right, wxID_ANY, "Size:");
    // wxStaticText *sizeValue = new wxStaticText(panel_right, wxID_ANY, "");
    // wxStaticText *dimensionsLabel = new wxStaticText(panel_right, wxID_ANY, "Dimensions:");

    // wxStaticText *kindValue = new wxStaticText(panel_right, wxID_ANY, "");
    // wxStaticText *sizeValue = new wxStaticText(panel_right, wxID_ANY, "");
    // dimensionsValue = new wxStaticText(panel_right, wxID_ANY, "");

    wxBoxSizer *rowSizer = nullptr;

    // Create a main vertical sizer to contain all the rows
    propertiesSizer = new wxBoxSizer(wxVERTICAL);

    // Load the bitmap
    wxString iconPath = "/home/jp/Downloads/wx_file_manager_project/src/resource/folder_icon.png";
    wxBitmap bufferedBitmap(iconPath, wxBITMAP_TYPE_ANY);
    wxBitmap resizedBitmap = wxBitmap(ResizeImageToWidth(bufferedBitmap, 100).ConvertToImage());
    bitmapControl = new wxStaticBitmap(panel_right, wxID_ANY, resizedBitmap);

    propertiesSizer->Add(bitmapControl, 1, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(nameLabel, 0, wxRIGHT, margin);
    rowSizer->Add(nameValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(sizeLabel, 0, wxRIGHT, margin);
    rowSizer->Add(sizeValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(lastModifiedLabel, 0, wxRIGHT, margin);
    rowSizer->Add(lastModifiedValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(permissionsLabel, 0, wxRIGHT, margin);
    rowSizer->Add(permissionsValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    MyFrame::MakePropertiesPanelTextEmpty();

    StyledButton *zoomInButton = new StyledButton(panel_right, wxID_ANY, wxT("\u002B"));
    StyledButton *zoomOutButton = new StyledButton(panel_right, wxID_ANY, wxT("\u2212"));

    zoomInButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                       {
        if (bitmap)
        {
            // wxImage image = bitmap->GetBitmap().ConvertToImage();
            // image.Rescale(image.GetWidth() * 1.1, image.GetHeight() * 1.1);
            // SetScaledImage(image, true);
            bitmap->ZoomIn();
        } });

    zoomOutButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                        {
        if (bitmap)
        {
            // wxImage image = bitmap->GetBitmap().ConvertToImage();
            // image.Rescale(image.GetWidth() * 0.9, image.GetHeight() * 0.9);
            // SetScaledImage(image, true);
            bitmap->ZoomOut();
        } });

    propertiesSizer->Add(zoomInButton, 0, wxEXPAND | wxALL, margin);
    propertiesSizer->Add(zoomOutButton, 0, wxEXPAND | wxALL, margin);

    quickAccessPanel = new FolderTreeStructurePanel(this, panel_left);
    quickAccessSizer = new wxBoxSizer(wxVERTICAL);
    quickAccessSizer->Add(quickAccessPanel, 1, wxEXPAND | wxRIGHT, 5);

    folderStructureSizer = new wxWrapSizer(wxHORIZONTAL);
    PopulateFolderIcons(rootPath, folderStructureSizer);

    this->SetSizerAndFit(mainSizer);
    panel_top->SetSizer(top_grid);
    panel_right->SetSizer(propertiesSizer);
    propertiesSizer->Layout();
    panel_left->SetSizer(quickAccessSizer);
    panel_mid->SetSizer(folderStructureSizer);

    searchResultList->Hide();

    Bind(wxEVT_SIZE, &MyFrame::OnSize, this);
    Bind(wxEVT_MOVE, &MyFrame::OnMove, this);
    Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyFrame::OnItemActivatedAtSearchResult, this);
    panel_mid->Bind(wxEVT_LEFT_DOWN, [&](wxMouseEvent &event)
                    {
        searchResultList->Hide();
        MakePropertiesPanelTextEmpty();
        event.Skip(); });

    wxInitAllImageHandlers();
}

void MyFrame::OnSearch(wxCommandEvent &event)
{
    wxString searchQuery = searchBar->GetValue();
    searchResultList->DeleteAllItems();

    //---- hide list while no queries ----------------
    searchResultList->Hide();
    //------------------------------------------------

    /*if (searchQuery != "")
    {
        searchFilesRecursively(searchQuery.ToStdString());
        for (const auto &item : allSearchItems)
        {
            if (item.Lower().Contains(searchQuery.Lower()))
            {
                wxVector<wxVariant> data;
                data.push_back(wxVariant(item));
                searchResultList->AppendItem(data);
            }
        }

        // Show the result list and position it below the search control
        searchResultList->Show();
    }
    else
    {
        searchResultList->Hide();
    }

    UpdateSearchResultPosition();
    */
}

void MyFrame::OnSearchButton(wxCommandEvent &event)
{
    wxString searchQuery = searchBar->GetValue();
    searchResultList->DeleteAllItems();

    if (searchQuery != "")
    {
        searchFilesRecursively(searchQuery.ToStdString());
        for (const auto &item : allSearchItems)
        {
            if (item.Lower().Contains(searchQuery.Lower()))
            {
                wxVector<wxVariant> data;
                data.push_back(wxVariant(item));
                searchResultList->AppendItem(data);
            }
        }

        // Show the result list and position it below the search control
        searchResultList->Show();
    }
    else
    {
        searchResultList->Hide();
    }

    UpdateSearchResultPosition();
}

void MyFrame::OnSize(wxSizeEvent &event)
{
    event.Skip(); // Ensure the default handling of the event
    UpdateSearchResultPosition();
}

void MyFrame::OnMove(wxMoveEvent &event)
{
    event.Skip(); // Ensure the default handling of the event
    UpdateSearchResultPosition();
}

void MyFrame::UpdateSearchResultPosition()
{
    if (searchResultList->IsShown())
    {
        wxPoint searchBarPos = searchBar->GetPosition();
        wxSize searchBarSize = searchBar->GetSize();
        wxPoint panelPos = searchBar->GetParent()->ClientToScreen(wxPoint(0, 0));
        wxPoint searchResultListPos = wxPoint(searchBarPos.x, searchBarPos.y + searchBarSize.GetHeight());
        searchResultList->SetPosition(searchResultListPos);

        // Get the number of items in the result list
        int itemCount = searchResultList->GetItemCount();
        if (itemCount == 0)
        {
            searchResultColumn->SetTitle("No Results Found");
        }
        else
        {
            searchResultColumn->SetTitle("Results");
        }
        int itemHeight = FromDIP(28);
        int searchResultListHeight = itemCount * itemHeight;
        searchResultList->SetSize(searchBarSize.GetWidth(), searchResultListHeight + FromDIP(28));
    }
}

void MyFrame::OnItemActivatedAtSearchResult(wxDataViewEvent &event)
{
    wxDataViewItem item = event.GetItem();
    wxDataViewListStore *store = static_cast<wxDataViewListStore *>(searchResultList->GetModel());

    wxVariant value;
    store->GetValue(value, item, 0);

    wxString itemValue = value.GetString();
    wxString prefix = itemValue.Mid(0, 6);
    wxString filePrefix = "[FILE]";
    wxString dirPrefix = "[DIR] ";

    if (prefix == filePrefix)
    {
        wxString fileName = itemValue.Mid(7);
        openFile(fileName);
    }
    else if (prefix == dirPrefix)
    {
        wxString dirName = itemValue.Mid(6);
        NavigateTo(dirName);
        OnFolderPathChange(dirName);
    }
}

wxBitmap MyFrame::ResizeImageToWidth(const wxBitmap &originalBitmap, int newWidth)
{
    // Get the original image from the bitmap
    wxImage originalImage = originalBitmap.ConvertToImage();

    // Calculate the new height while maintaining the aspect ratio
    int originalWidth = originalImage.GetWidth();
    int originalHeight = originalImage.GetHeight();
    int newHeight = (originalHeight * newWidth) / originalWidth;

    // Rescale the image to the new width and height
    wxImage resizedImage = originalImage.Scale(newWidth, newHeight, wxIMAGE_QUALITY_HIGH);

    // Convert the resized image back to a bitmap
    wxBitmap resizedBitmap(resizedImage);

    return resizedBitmap;
}

void MyFrame::HidePropertiesPanel()
{
    MyFrame::panel_right->Hide();
    MyFrame::panel_right->SetSize(0, 0);
    MyFrame::panel_right->Layout();
    MyFrame::panel_bottom->Layout();
}

void MyFrame::ShowPropertiesPanel()
{
    MyFrame::panel_right->Show();
    MyFrame::panel_right->Layout();
}

void MyFrame::PopulateFolderIcons(const wxString &path, wxSizer *sizer)
{
    wxDir dir(path);
    if (!dir.IsOpened())
    {
        wxLogError("Failed to open directory: %s", path);
        return;
    }

    currentPath = path;
    nameText->SetValue(path);

    sizer->Clear(true);

    wxString filename;
    bool cont = dir.GetFirst(&filename);
    std::vector<wxString> folders;
    std::vector<wxString> files;

    while (cont)
    {
        if (!filename.StartsWith(".")) // Skip hidden folders
        {
            wxString fullPath = wxFileName::DirName(path).GetFullPath() + filename;

            if (wxFileName::DirExists(fullPath))
            {
                folders.push_back(filename);
            }
            else
            {
                files.push_back(filename);
            }
        }
        cont = dir.GetNext(&filename);
    }

    std::sort(folders.begin(), folders.end());
    std::sort(files.begin(), files.end());

    for (const auto &folder : folders)
    {
        wxString fullPath = wxFileName::DirName(path).GetFullPath() + folder;
        std::string folderNameStr = folder.ToStdString();
        FolderIcon *folderIcon = new FolderIcon(panel_mid, folderNameStr, fullPath, wxSize(70, 70));
        sizer->Add(folderIcon, 0, wxALL, margin);
        sizer->Layout();
        folderIcon->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent &event)
                         {
                            wxString folderPath = folderIcon->GetFolderPath();
                            OnFolderPathChange(folderPath); });
    }

    for (const auto &file : files)
    {
        wxString fullPath = wxFileName::DirName(path).GetFullPath() + file;
        std::string fileNameStr = file.ToStdString();
        FileIcon *fileIcon = new FileIcon(panel_mid, fileNameStr, fullPath, wxSize(70, 70));
        sizer->Add(fileIcon, 0, wxALL, margin);
        sizer->Layout();
        fileIcon->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent &event)
                       {
                           wxString filePath = fileIcon->GetFilePath();
                           openFile(filePath); });
    }
}

void MyFrame::OnFolderPathChange(wxString folderPath)
{
    NavigateTo(folderPath);
    PopulateFolderIcons(folderPath, folderStructureSizer);
}

wxString MyFrame::formatTime(std::filesystem::file_time_type ftime)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&cftime), "%F %T");
    return wxString(ss.str());
}

wxString MyFrame::getPermissions(std::filesystem::perms p) const
{
    wxString permStr;
    permStr += ((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-");
    permStr += ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-");
    permStr += ((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-");
    permStr += ((p & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-");
    permStr += ((p & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-");
    permStr += ((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-");
    permStr += ((p & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-");
    permStr += ((p & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-");
    permStr += ((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-");
    return permStr;
}

wxString MyFrame::formatSize(uintmax_t size)
{
    static const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int suffixIndex = 0;
    double fileSize = static_cast<double>(size);

    while (fileSize >= 1024.0 && suffixIndex < sizeof(suffixes) / sizeof(suffixes[0]) - 1)
    {
        fileSize /= 1024.0;
        suffixIndex++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << fileSize << " " << suffixes[suffixIndex];
    return wxString(ss.str());
}

std::vector<wxString> MyFrame::GetFilesAndFolders()
{
    std::vector<wxString> result;
    wxDir dir(currentPath);
    wxString filename;
    bool cont = dir.GetFirst(&filename);
    while (cont)
    {
        result.push_back(filename);
        cont = dir.GetNext(&filename);
    }
    return result;
}

Props MyFrame::GetFileProperties(const wxString &fileName)
{
    Props newProps("", "", "", "");
    std::filesystem::path filePath = std::filesystem::path(currentPath.ToStdString()) / fileName.ToStdString();
    if (std::filesystem::exists(filePath))
    {
        auto ftime = std::filesystem::last_write_time(filePath);
        auto s = std::filesystem::status(filePath);

        std::cout << "File: " << wxString(filePath.filename().string()) << std::endl;
        std::cout << "Size: " << formatSize(std::filesystem::file_size(filePath)) << std::endl;
        std::cout << "Last modified: " << formatTime(ftime) << std::endl;
        std::cout << "Permissions: " << getPermissions(s.permissions()) << std::endl;
        newProps.Name = wxString(filePath.filename().string());
        newProps.Size = formatSize(std::filesystem::file_size(filePath));
        newProps.LastModified = formatTime(ftime);
        newProps.Permissions = getPermissions(s.permissions());

        // Store recently accessed file
        recentlyAccessedFiles[wxString(filePath.string())] = std::chrono::system_clock::now();
    }
    else
    {
        std::cout << "File does not exist." << std::endl;
    }

    return newProps;
}

bool MyFrame::openFile(const wxString &fName)
{
    std::string fileName = fName.ToStdString();
    std::filesystem::path filePath = std::filesystem::path(currentPath.ToStdString()) / fileName;
    if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath))
    {
        if (std::filesystem::is_regular_file(filePath))
        {

            recentlyAccessedFiles[wxString(filePath)] = std::chrono::system_clock::now();
            recentlyAccessedFolders.push_back(wxFileName(wxString(filePath)).GetPath());
            std::string command;
#ifdef _WIN32
            command = std::string("start '") + filePath.string() + "'";
#elif __linux__
            command = std::string("xdg-open '") + filePath.string() + "'";
#elif __APPLE__
            command = std::string("open '") + filePath.string() + "'";
#endif
            system(command.c_str());
            return true;
        }
        else
        {
            std::cerr << "Error: Not a regular file!" << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "File does not exist or is a directory." << std::endl;
        return false;
    }
}

void MyFrame::createFile(const wxString &fileName)
{
    wxString filePath = currentPath + wxFileName::GetPathSeparator() + fileName;

    if (wxFileExists(filePath))
    {
        char choice;
        std::cout << "File " << filePath << " already exists. Do you want to overwrite it? (y/n): ";
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y')
        {
            std::cout << "File creation aborted." << std::endl;
            return;
        }
    }

    std::ofstream ofs(filePath.ToStdString());
    if (ofs)
    {
        std::cout << "File created: " << fileName << std::endl;
    }
    else
    {
        std::cout << "Failed to create file: " << fileName << std::endl;
    }
}

void MyFrame::createDirectory(const wxString &dirName)
{
    wxString dirPath = currentPath + wxFileName::GetPathSeparator() + dirName;

    if (wxDirExists(dirPath))
    {
        std::cout << "Directory " << dirPath << " already exists." << std::endl;
        return;
    }

    if (wxMkdir(dirPath))
    {
        std::cout << "Directory created: " << dirName << std::endl;
    }
    else
    {
        std::cout << "Failed to create directory: " << dirName << std::endl;
    }
}

void MyFrame::deleteFile(const wxString &fileName)
{
    std::filesystem::path filePath = std::filesystem::path(currentPath.ToStdString()) / fileName.ToStdString();
    if (std::filesystem::exists(filePath))
    {
        std::filesystem::remove(filePath);
        std::cout << "File deleted: " << filePath << std::endl;
    }
    else
    {
        std::cout << "File does not exist: " << filePath << std::endl;
    }
    MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
}

void MyFrame::deleteDirectory(const wxString &dirName)
{
    std::filesystem::path dirPath = std::filesystem::path(currentPath.ToStdString()) / dirName.ToStdString();
    if (std::filesystem::exists(dirPath))
    {
        std::filesystem::remove_all(dirPath);
        std::cout << "Directory deleted: " << dirPath << std::endl;
    }
    else
    {
        std::cout << "Directory does not exist: " << dirPath << std::endl;
    }
    MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
}

void MyFrame::renameFile(const wxString &oldName, const wxString &newName)
{
    std::filesystem::path oldPath = std::filesystem::path(currentPath.ToStdString()) / oldName.ToStdString();
    std::filesystem::path newPath = std::filesystem::path(currentPath.ToStdString()) / newName.ToStdString();
    if (std::filesystem::exists(oldPath))
    {
        std::filesystem::rename(oldPath, newPath);
        MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
        std::cout << "File renamed from " << oldPath.filename() << " to " << newPath.filename() << std::endl;
    }
    else
    {
        std::cout << "File does not exist: " << oldPath << std::endl;
    }
}

void MyFrame::renameDirectory(const wxString &oldName, const wxString &newName)
{
    std::filesystem::path oldPath = std::filesystem::path(currentPath.ToStdString()) / oldName.ToStdString();
    std::filesystem::path newPath = std::filesystem::path(currentPath.ToStdString()) / newName.ToStdString();
    if (std::filesystem::exists(oldPath))
    {
        std::filesystem::rename(oldPath, newPath);
        MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);

        std::cout << "Directory renamed from " << oldName << " to " << newName << std::endl;
    }
    else
    {
        std::cout << "Directory does not exist: " << oldPath << std::endl;
    }
}

void MyFrame::searchFilesRecursively(const wxString &query)
{
    wxString regexQuery(query, wxRE_ICASE); // Case-insensitive regex
    wxString filename;
    wxDir dir(currentPath);
    bool cont = dir.GetFirst(&filename, wxString(), wxDIR_FILES | wxDIR_DIRS | wxDIR_DOTDOT);
    while (cont)
    {
        if (wxString(filename).Matches(regexQuery))
        {
            allSearchItems.Add((wxDirExists(currentPath + wxFileName::GetPathSeparator() + filename) ? "[DIR] " : "[FILE] "));
        }
        cont = dir.GetNext(&filename);
    }
}

void MyFrame::searchFilesRecursively(const std::string &query)
{
    allSearchItems.Clear();
    std::regex regexQuery(query, std::regex_constants::icase); // Case-insensitive regex
    for (const auto &entry : std::filesystem::recursive_directory_iterator(currentPath.ToStdString()))
    {
        if (std::regex_search(entry.path().filename().string(), regexQuery))
        {
            allSearchItems.Add((entry.is_directory() ? "[DIR] " : "[FILE] ") + entry.path().string());
        }
    }
}

void MyFrame::NavigateTo(const wxString &path)
{
    if (currentPath != path)
    {
        if (!currentPath.empty())
        {
            backwardStack.push(currentPath);
        }
        currentPath = path;
        std::cout << "Navigated to directory: " << path << std::endl;
        while (!forwardStack.empty())
        {
            forwardStack.pop();
        }
    }
    else
    {
        std::cout << "Already in the directory: " << path.ToStdString() << std::endl;
    }
}

void MyFrame::navigateBack()
{
    if (!backwardStack.empty())
    {
        forwardStack.push(currentPath);
        currentPath = backwardStack.top();
        backwardStack.pop();
    }
    else
    {
        std::cout << "No more history to go back." << std::endl;
    }
}

void MyFrame::navigateForward()
{
    if (!forwardStack.empty())
    {
        backwardStack.push(currentPath);
        currentPath = forwardStack.top();
        forwardStack.pop();
    }
    else
    {
        std::cout << "No more history to go forward." << std::endl;
    }
}

void MyFrame::gotoParentDirectory()
{
    wxString parentPath = wxFileName(currentPath).GetPath();
    if (!currentPath.empty() && parentPath != currentPath)
    {
        NavigateTo(parentPath);
    }
    else
    {
        std::cout << "Already at the root directory." << std::endl;
    }
}

void MyFrame::navigateToHome()
{
    wxString homePath = wxGetHomeDir();
    if (!currentPath.empty() && homePath != currentPath)
    {
        NavigateTo(homePath);
    }
    else
    {
        std::cout << "Already at the root directory." << std::endl;
    }
}

void MyFrame::displayRecentlyAccessedFolders()
{
    std::cout << "Recently accessed folders:" << std::endl;
    for (const auto &folder : recentlyAccessedFolders)
    {
        wxString folderPath = currentPath + wxFileName::GetPathSeparator() + folder;
        if (wxDirExists(folderPath))
        {
            std::filesystem::path fpath(folderPath.ToStdString());
            auto ftime = std::filesystem::last_write_time(fpath);
            auto sctp = std::chrono::time_point_cast<std::filesystem::file_time_type::clock::duration>(
                recentlyAccessedFiles[folderPath]);
            std::filesystem::last_write_time(fpath,
                                             std::filesystem::file_time_type{sctp.time_since_epoch()});
            std::cout << folderPath << " - " << formatTime(ftime) << std::endl;
        }
    }
}

void MyFrame::displayRecentlyAccessedFiles()
{
    std::cout << "Recently accessed files:" << std::endl;
    for (const auto &pair : recentlyAccessedFiles)
    {
        wxString filePath = pair.first;
        auto ftime = std::filesystem::last_write_time(filePath.ToStdString());
        auto sctp = std::chrono::time_point_cast<std::filesystem::file_time_type::clock::duration>(
            pair.second);
        std::filesystem::last_write_time(filePath.ToStdString(),
                                         std::filesystem::file_time_type{sctp.time_since_epoch()});
        std::cout << filePath << " - " << formatTime(ftime) << std::endl;
    }
}

bool MyFrame::setFilePermissions(const wxString &filename, int permissions)
{
    wxString filePath = currentPath + wxFileName::GetPathSeparator() + filename;
    if (wxFileExists(filePath))
    {
        try
        {
            std::filesystem::permissions(filePath.ToStdString(), std::filesystem::perms(permissions));
            std::cout << "File permissions changed successfully.\n";
            return true;
        }
        catch (const std::filesystem::filesystem_error &ex)
        {
            std::cerr << "Failed to change file permissions: " << ex.what() << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "File does not exist: " << filename << std::endl;
        return false;
    }
}

//---------------------------------------------Context Menu Functions ----------------------------------------------------
void MyFrame::OnShowContextMenu(wxContextMenuEvent &event)
{
    wxMenu menu;
    bool isFolder = false;
    bool isFile = false;
    FileIcon *activeFileIcon = nullptr;
    FolderIcon *activeFolderIcon = nullptr;

    wxWindowList &children = panel_mid->GetChildren();
    for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
    {
        if (FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it); folderIcon && folderIcon->isActive())
        {
            isFolder = true;
            activeFolderIcon = folderIcon;
        }
        else if (FileIcon *fileIcon = dynamic_cast<FileIcon *>(*it); fileIcon && fileIcon->isActive())
        {
            isFile = true;
            activeFileIcon = fileIcon;
        }
    }

    if (!isFolder && !isFile)
    {
        wxMenuItem *newFileItem = menu.Append(wxID_ANY, "New File");
        Bind(wxEVT_MENU, &MyFrame::OnNewFile, this, newFileItem->GetId());

        wxMenuItem *newFolderItem = menu.Append(wxID_ANY, "New Folder");
        Bind(wxEVT_MENU, &MyFrame::OnNewFolder, this, newFolderItem->GetId());
    }

    if (isFolder)
    {
        wxMenuItem *openItem = menu.Append(wxID_ANY, "Open");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnOpen(activeFolderIcon); }, openItem->GetId());
        wxMenuItem *cutItem = menu.Append(wxID_ANY, "Cut");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnCut(activeFolderIcon); }, cutItem->GetId());
        wxMenuItem *copyItem = menu.Append(wxID_ANY, "Copy");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnCopy(activeFolderIcon); }, copyItem->GetId());
        wxMenuItem *pasteItem = menu.Append(wxID_ANY, "Paste");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &)
             { OnPaste(activeFolderIcon->GetFolderPath()); });
        pasteItem->Enable(isCut || !copyBuffer.IsEmpty());

        menu.AppendSeparator();
        wxMenuItem *deleteItem = menu.Append(wxID_ANY, "Delete");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnDelete(activeFolderIcon); }, deleteItem->GetId());
        wxMenuItem *renameItem = menu.Append(wxID_ANY, "Rename");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnRename(activeFolderIcon); }, renameItem->GetId());
        menu.AppendSeparator();
        wxMenuItem *propertiesItem = menu.Append(wxID_ANY, "Properties");
        Bind(wxEVT_MENU, [this, activeFolderIcon](wxCommandEvent &event)
             { OnProperties(activeFolderIcon); }, propertiesItem->GetId());
    }
    else if (isFile)
    {
        wxMenuItem *openItem = menu.Append(wxID_ANY, "Open");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &event)
             { OnOpen(activeFileIcon); }, openItem->GetId());
        wxMenuItem *cutItem = menu.Append(wxID_ANY, "Cut");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &event)
             { OnCut(activeFileIcon); }, cutItem->GetId());
        wxMenuItem *copyItem = menu.Append(wxID_ANY, "Copy");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &event)
             { OnCopy(activeFileIcon); }, copyItem->GetId());
        wxMenuItem *pasteItem = menu.Append(wxID_ANY, "Paste");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &)
             { OnPaste(currentPath); }, pasteItem->GetId());
        pasteItem->Enable(isCut || !copyBuffer.IsEmpty());
        menu.AppendSeparator();
        wxMenuItem *deleteItem = menu.Append(wxID_ANY, "Delete");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &event)
             { OnDelete(activeFileIcon); }, deleteItem->GetId());
        wxMenuItem *renameItem = menu.Append(wxID_ANY, "Rename");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &)
             { OnRename(activeFileIcon); }, renameItem->GetId());
        menu.AppendSeparator();
        wxMenuItem *propertiesItem = menu.Append(wxID_ANY, "Properties");
        Bind(wxEVT_MENU, [this, activeFileIcon](wxCommandEvent &)
             { OnProperties(activeFileIcon); }, propertiesItem->GetId());
    }
    else
    {
        wxMenuItem *openItem = menu.Append(wxID_ANY, "Open");
        openItem->Enable(false);
        wxMenuItem *cutItem = menu.Append(wxID_ANY, "Cut");
        cutItem->Enable(false);
        wxMenuItem *copyItem = menu.Append(wxID_ANY, "Copy");
        copyItem->Enable(false);
        wxMenuItem *pasteItem = menu.Append(wxID_ANY, "Paste");
        Bind(wxEVT_MENU, [this](wxCommandEvent &)
             { OnPaste(currentPath); }, pasteItem->GetId());
        pasteItem->Enable(isCut || !copyBuffer.IsEmpty());
        menu.AppendSeparator();
        wxMenuItem *deleteItem = menu.Append(wxID_ANY, "Delete");
        deleteItem->Enable(false);
        wxMenuItem *renameItem = menu.Append(wxID_ANY, "Rename");
        renameItem->Enable(false);
        menu.AppendSeparator();
        wxMenuItem *propertiesItem = menu.Append(wxID_ANY, "Properties");
        Bind(wxEVT_MENU, &MyFrame::OnProperties, this, propertiesItem->GetId());
    }

    PopupMenu(&menu);
}

void MyFrame::OnOpen(FolderIcon *folderIcon)
{
    wxString folderPath = folderIcon->GetFolderPath();
    NavigateTo(folderPath);
    OnFolderPathChange(folderPath);
}

void MyFrame::OnOpen(FileIcon *fileIcon)
{
    wxString filePath = fileIcon->GetFilePath();
    openFile(filePath);
}

void MyFrame::OnCut(FolderIcon *folderIcon)
{
    wxString folderPath = folderIcon->GetFolderPath();
    cutBuffer = folderPath;
    folderIcon->SetIconOpacity(128);
    isCut = true;
}

void MyFrame::OnCut(FileIcon *fileIcon)
{
    wxString filePath = fileIcon->GetFilePath();
    cutBuffer = filePath;
    fileIcon->SetIconOpacity(128);
    isCut = true;
}

void MyFrame::OnCopy(FolderIcon *folderIcon)
{
    wxString folderPath = folderIcon->GetFolderPath();
    copyBuffer = folderPath;
    isCut = false;
}

void MyFrame::OnCopy(FileIcon *fileIcon)
{
    wxString filePath = fileIcon->GetFilePath();
    copyBuffer = filePath;
    isCut = false;
}

void MyFrame::OnPaste(wxString folderPath)
{
    if (isCut)
    {
        if (wxDirExists(cutBuffer))
        {
            wxRenameFile(cutBuffer, folderPath + wxFileName::GetPathSeparator() + wxFileName(cutBuffer).GetFullName());
            std::cout << "Cut and pasted folder " << cutBuffer << " to " << folderPath << std::endl;
            cutBuffer.Clear();
            isCut = false;
        }
        else if (wxFileExists(cutBuffer))
        {
            wxRenameFile(cutBuffer, folderPath + wxFileName::GetPathSeparator() + wxFileName(cutBuffer).GetFullName());
            std::cout << "Cut and pasted file " << cutBuffer << " to " << folderPath << std::endl;
            cutBuffer.Clear();
            isCut = false;
        }
        else
        {
            std::cout << "Invalid path: " << cutBuffer << std::endl;
            cutBuffer.Clear();
        }
    }
    else
    {
        if (wxDirExists(copyBuffer))
        {
            wxCopyFile(copyBuffer, folderPath + wxFileName::GetPathSeparator() + wxFileName(copyBuffer).GetFullName());
            std::cout << "Copied folder " << copyBuffer << " to " << folderPath << std::endl;
            copyBuffer.Clear();
        }
        else if (wxFileExists(copyBuffer))
        {
            wxCopyFile(copyBuffer, folderPath + wxFileName::GetPathSeparator() + wxFileName(copyBuffer).GetFullName());
            std::cout << "Copied file " << copyBuffer << " to " << folderPath << std::endl;
            copyBuffer.Clear();
        }
        else
        {
            std::cout << "Invalid path: " << copyBuffer << std::endl;
            copyBuffer.Clear();
        }
    }
    MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
}

void MyFrame::OnDelete(FolderIcon *folderIcon)
{
    wxString folderPath = folderIcon->GetFolderPath();
    deleteDirectory(folderPath);
}

void MyFrame::OnDelete(FileIcon *fileIcon)
{
    wxString filePath = fileIcon->GetFilePath();
    deleteFile(filePath);
}

void MyFrame::OnRename(FolderIcon *folderIcon)
{
    wxString oldName = folderIcon->GetFolderName();
    wxString newName = wxGetTextFromUser("Enter new folder name:", "Rename Folder", oldName, this);
    if (!newName.IsEmpty())
    {
        renameDirectory(oldName, newName);
    }
}

void MyFrame::OnRename(FileIcon *fileIcon)
{
    wxString oldName = fileIcon->GetFileName();
    wxString newName = wxGetTextFromUser("Enter new file name:", "Rename File", oldName, this);
    if (!newName.IsEmpty())
    {
        renameFile(oldName, newName);
    }
}

void MyFrame::OnProperties(FolderIcon *folderIcon)
{
    wxString folderName = folderIcon->GetFolderPath();
    std::cout << "OnProperties...enter" << std::endl;
    Props props = GetDirectoryProperties(folderName);
    std::cout << "OnProperties...exit" << std::endl;
    UpdatePropertiesUI(props);
    std::cout << "OnProperties...exitUIUpdate..." << std::endl;
}

void MyFrame::OnProperties(FileIcon *fileIcon)
{
    wxString fileName = fileIcon->GetFilePath();
    Props props = GetFileProperties(fileName);
    wxString message = wxString::Format("Name: %s\nSize: %s\nLast Modified: %s\nPermissions: %s",
                                        props.Name, props.Size, props.LastModified, props.Permissions);
    wxMessageBox(message, "File Properties", wxOK | wxICON_INFORMATION);
}

// void MyFrame::OnProperties(wxCommandEvent &event)
// {
//     wxString folderName = MyFrame::currentPath;
//     std::cout << "OnProperties...enter" << std::endl;
//     Props props = GetDirectoryProperties(folderName);
//     wxString message = wxString::Format("Name: %s\nSize: %s\nLast Modified: %s\nPermissions: %s",
//                                         props.Name, props.Size, props.LastModified, props.Permissions);
//     wxMessageBox(message, "Folder Properties", wxOK | wxICON_INFORMATION);
// }

void MyFrame::OnNewFolder(wxCommandEvent &event)
{
    wxString folderName = wxGetTextFromUser("Enter folder name:", "New Folder", "", this);
    if (!folderName.IsEmpty())
    {
        createDirectory(folderName);
        MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
    }
}

void MyFrame::OnNewFile(wxCommandEvent &event)
{
    wxString fileName = wxGetTextFromUser("Enter file name:", "New File", "", this);
    if (!fileName.IsEmpty())
    {
        createFile(fileName);
        MyFrame::PopulateFolderIcons(currentPath, folderStructureSizer);
    }
}
//------------------------------------------------------------------------------------------------------------------------

//----------------------------------Async Methods For Directory Properties Calc -------------------------------------------

uintmax_t MyFrame::GetDirectorySize(const std::filesystem::path &dirPath)
{
    if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
    {
        uintmax_t size = 0;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(dirPath, std::filesystem::directory_options::skip_permission_denied))
        {
            if (std::filesystem::is_regular_file(entry))
            {
                size += std::filesystem::file_size(entry);
            }
        }
        return size;
    }
    std::cerr << "Directory does not exist: " << dirPath << std::endl;
    return 0;
}

std::future<wxString> MyFrame::GetDirectorySizeAsync(const std::filesystem::path &dirPath)
{
    return std::async(std::launch::async, [dirPath, this]()
                      {
        try {
            uintmax_t size = GetDirectorySize(dirPath);
            return formatSize(size);
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            return wxString("Error calculating size");
        } });
}

std::future<wxString> MyFrame::GetLastWriteTimeAsync(const std::filesystem::path &dirPath)
{
    return std::async(std::launch::async, [dirPath, this]()
                      {
        try {
            auto ftime = std::filesystem::last_write_time(dirPath);
            return formatTime(ftime);
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            return wxString("Error getting last modified time");
        } });
}

std::future<wxString> MyFrame::GetPermissionsAsync(const std::filesystem::path &dirPath)
{
    return std::async(std::launch::async, [dirPath, this]()
                      {
        try {
            auto s = std::filesystem::status(dirPath);
            return getPermissions(s.permissions());
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            return wxString("Error getting permissions");
        } });
}

Props MyFrame::GetDirectoryProperties(const wxString &dirName)
{
    Props newProps("", "", "", "");
    std::filesystem::path dirPath = std::filesystem::path(currentPath.ToStdString()) / dirName.ToStdString();
    std::cout << "Dir Path in Props : " << dirPath << std::endl;
    if (std::filesystem::exists(dirPath))
    {
        std::cout << "checkpoint 1" << std::endl;
        auto ftimeFuture = GetLastWriteTimeAsync(dirPath);
        std::cout << "checkpoint 2" << std::endl;
        auto sizeFuture = GetDirectorySizeAsync(dirPath);
        std::cout << "checkpoint 3" << std::endl;
        auto permissionsFuture = GetPermissionsAsync(dirPath);
        std::cout << "checkpoint 4" << std::endl;

        // Get futures' results asynchronously
        wxString lastModified = ftimeFuture.get();
        std::cout << "checkpoint 5" << std::endl;
        wxString size = sizeFuture.get();
        std::cout << "checkpoint 6" << std::endl;
        wxString permissions = permissionsFuture.get();
        std::cout << "checkpoint 7" << std::endl;

        newProps.Name = wxString(dirPath.filename().string());
        std::cout << "checkpoint 8" << std::endl;
        newProps.Size = size;
        std::cout << "checkpoint 9" << std::endl;
        newProps.LastModified = lastModified;
        std::cout << "checkpoint 10" << std::endl;
        newProps.Permissions = permissions;
        std::cout << "checkpoint 11" << std::endl;

        recentlyAccessedFiles[wxString(dirPath.string())] = std::chrono::system_clock::now();
    }
    else
    {
        std::cout << "Directory : " << dirPath << " does not exist." << std::endl;
    }
    std::cout << "checkpoint 12" << std::endl;
    ShowPropertiesPanel();
    return newProps;
}

void MyFrame::CreateRightPanel()
{
    if (panel_right)
    {
        panel_right->Destroy();
    }

    panel_right = new wxScrolledWindow(panel_bottom, wxID_ANY);
    panel_right->SetScrollRate(10, 10);
    panel_right->SetMinSize(wxSize(300, 300));
    propertiesSizer = new wxBoxSizer(wxVERTICAL);

    // Initialize UI controls
    nameValue = new wxStaticText(panel_right, wxID_ANY, "");
    sizeValue = new wxStaticText(panel_right, wxID_ANY, "");
    lastModifiedValue = new wxStaticText(panel_right, wxID_ANY, "");
    permissionsValue = new wxStaticText(panel_right, wxID_ANY, "");

    nameLabel = new wxStaticText(panel_right, wxID_ANY, "Name:");
    sizeLabel = new wxStaticText(panel_right, wxID_ANY, "Size:");
    lastModifiedLabel = new wxStaticText(panel_right, wxID_ANY, "Last Modified:");
    permissionsLabel = new wxStaticText(panel_right, wxID_ANY, "Permissions:");

    wxBoxSizer *rowSizer = nullptr;

    // Load the bitmap
    wxString iconPath = "/home/jp/Downloads/wx_file_manager_project/src/resource/folder_icon.png";
    wxBitmap bufferedBitmap(iconPath, wxBITMAP_TYPE_ANY);
    wxBitmap resizedBitmap = wxBitmap(ResizeImageToWidth(bufferedBitmap, 100).ConvertToImage());
    bitmapControl = new wxStaticBitmap(panel_right, wxID_ANY, resizedBitmap);

    propertiesSizer->Add(bitmapControl, 1, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(nameLabel, 0, wxRIGHT, margin);
    rowSizer->Add(nameValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(sizeLabel, 0, wxRIGHT, margin);
    rowSizer->Add(sizeValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);
    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(lastModifiedLabel, 0, wxRIGHT, margin);
    rowSizer->Add(lastModifiedValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    rowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowSizer->Add(permissionsLabel, 0, wxRIGHT, margin);
    rowSizer->Add(permissionsValue, 1, wxEXPAND | wxLEFT, margin);
    propertiesSizer->Add(rowSizer, 0, wxEXPAND | wxALL, margin);

    StyledButton *zoomInButton = new StyledButton(panel_right, wxID_ANY, wxT("\u002B"));
    StyledButton *zoomOutButton = new StyledButton(panel_right, wxID_ANY, wxT("\u2212"));

    propertiesSizer->Add(zoomInButton, 0, wxEXPAND | wxALL, margin);
    propertiesSizer->Add(zoomOutButton, 0, wxEXPAND | wxALL, margin);

    zoomInButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                       {
        if (bitmapControl)
        {
            wxImage image = bitmapControl->GetBitmap().ConvertToImage();
            image.Rescale(image.GetWidth() * 1.1, image.GetHeight() * 1.1, wxIMAGE_QUALITY_HIGH);
            bitmapControl->SetBitmap(wxBitmap(image));
            propertiesSizer->Layout();
        } });

    zoomOutButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event)
                        {
        if (bitmapControl)
        {
            wxImage image = bitmapControl->GetBitmap().ConvertToImage();
            image.Rescale(image.GetWidth() * 0.9, image.GetHeight() * 0.9, wxIMAGE_QUALITY_HIGH);
            bitmapControl->SetBitmap(wxBitmap(image));
            propertiesSizer->Layout();
        } });

    panel_right->SetSizer(propertiesSizer);
    sizer_bottom->Add(panel_right, 0, wxEXPAND);
    sizer_bottom->Layout();
    propertiesSizer->Layout();
}

void MyFrame::OnProperties(wxCommandEvent &event)
{
    wxString folderName = MyFrame::currentPath;
    std::cout << "OnProperties...enter" << std::endl;
    Props props = GetDirectoryProperties(folderName);
    UpdatePropertiesUI(props);
}

void MyFrame::UpdatePropertiesUI(const Props &props)
{
    std::cout << "UpdatePropertiesUI...enter" << std::endl;
    nameLabel->SetLabel("Name: ");
    sizeLabel->SetLabel("Size: ");
    lastModifiedLabel->SetLabel("Last Modified: ");
    permissionsLabel->SetLabel("Permissions: ");
    try
    {
        std::cout << "Props Name : " << props.Name << std::endl;
        if (nameValue)
        {
            std::cout << "checkpoint 0" << std::endl;
            nameValue->SetLabel(props.Name);
        }
        std::cout << "checkpoint 1" << std::endl;
        if (sizeValue)
        {
            sizeValue->SetLabel(props.Size);
        }
        std::cout << "checkpoint 2" << std::endl;
        if (lastModifiedValue)
        {
            lastModifiedValue->SetLabel(props.LastModified);
        }
        std::cout << "checkpoint 3" << std::endl;
        if (permissionsValue)
        {
            permissionsValue->SetLabel(props.Permissions);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    std::cout << "UpdatePropertiesUI...exit" << std::endl;
    // Refresh the frame to ensure the changes are displayed
    this->Layout();
    this->Refresh();
}

//------------------------------------------------------------------------------------------------------------------------

void MyFrame::MakePropertiesPanelTextEmpty()
{
    nameLabel->SetLabel("");
    sizeLabel->SetLabel("");
    lastModifiedLabel->SetLabel("");
    permissionsLabel->SetLabel("");
    if (nameValue)
    {
        nameValue->SetLabel("");
    }
    if (sizeValue)
    {
        sizeValue->SetLabel("");
    }
    if (lastModifiedValue)
    {
        lastModifiedValue->SetLabel("");
    }
    if (permissionsValue)
    {
        permissionsValue->SetLabel("");
    }
    HidePropertiesPanel();
}