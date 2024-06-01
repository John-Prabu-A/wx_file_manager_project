#ifndef MYFRAME_H
#define MYFRAME_H

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
#include <wx/filefn.h>
#include <wx/datetime.h>
#include <wx/regex.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/slider.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stack>
#include <filesystem>
#include <map>
#include <ctime>
#include <algorithm>
#include <regex>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/dataview.h>
#include <functional>
#include <future>
#include <wx/msgdlg.h>

#include <wx/mimetype.h>
#include <unordered_map>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#elif defined(__linux__)
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <cstdlib>
#include <CoreServices/CoreServices.h>
#endif

#include "FolderIcon.h"
#include "FileIcon.h"
#include "BufferedBitmap.h"
#include "folderTreeStructure.h"
#include "StyledButton.h"
// #include "Tries.h"

class BufferedBitmap;
class FolderIcon;
class Props
{
public:
    wxString Name;
    wxString Size;
    wxString LastModified;
    wxString Permissions;

    Props(const wxString &name = "Unknown", const wxString &size = "Calculating...",
          const wxString &lastModified = "Calculating...", const wxString &permissions = "Calculating...")
        : Name(name), Size(size), LastModified(lastModified), Permissions(permissions) {}
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size, const wxString &path = "");
    // void SetScaledImage(const wxImage &image, bool isDefaultPath = false);
    // void OnResize(wxSizeEvent &event);
    void HidePropertiesPanel();
    void ShowPropertiesPanel();
    void CreateRightPanel();
    wxString GetTrashDirectory();
    void PopulateFolderIcons(const wxString &path, wxSizer *sizer);
    void UpdateFolders(wxCommandEvent &event);
    void OnFolderPathChange(wxString folderPath);
    void displayDirectoryProperties(const wxString &dirName);
    bool setFilePermissions(const wxString &filename, int permissions);
    void displayRecentlyAccessedFiles();
    void displayRecentlyAccessedFolders();
    void navigateToHome();
    void gotoParentDirectory();
    void navigateForward();
    void navigateBack();
    void NavigateTo(const wxString &path);
    void searchFilesRecursively(const wxString &query);
    void searchFilesRecursively(const std::string &query);
    void renameDirectory(const wxString &oldName, const wxString &newName);
    void renameFile(const wxString &oldName, const wxString &newName);
    void deleteDirectory(const wxString &dirName);
    void deleteFile(const wxString &fileName);
    void createDirectory(const wxString &dirName);
    void createFile(const wxString &fileName);
    void OpenFile(const wxString &filePath);
    std::vector<wxString> GetFilesAndFolders();
    Props GetFileProperties(const wxString &fileName);
    wxBitmap ResizeImageToWidth(const wxBitmap &originalBitmap, int newWidth);
    void setPropertiesIcon(const wxString &iconPath);

    // ------------Getting Directory Properties Async -----------------------------------
    uintmax_t GetDirectorySize(const std::filesystem::path &dirPath);
    std::future<wxString> GetDirectorySizeAsync(const std::filesystem::path &dirPath);
    std::future<wxString> GetLastWriteTimeAsync(const std::filesystem::path &dirPath);
    std::future<wxString> GetPermissionsAsync(const std::filesystem::path &dirPath);

    Props GetDirectoryProperties(const wxString &dirName);

    void OnProperties(wxCommandEvent &event);
    void UpdatePropertiesUI(const Props &props);
    void MakePropertiesPanelTextEmpty();
    //------------------------------------------------------------------------------------

    //---------context menu functions ----------------------------------------------------
    void OnShowContextMenu(wxContextMenuEvent &event);
    void OnOpen(FolderIcon *folderIcon);
    void OnOpen(FileIcon *fileIcon);
    void OnCut(FolderIcon *folderIcon);
    void OnCut(FileIcon *fileIcon);
    void OnCopy(FolderIcon *folderIcon);
    void OnCopy(FileIcon *fileIcon);
    void OnPaste(wxString folderPath);
    void OnDelete(FolderIcon *folderIcon);
    void OnDelete(FileIcon *fileIcon);
    void OnRename(FolderIcon *folderIcon);
    void OnRename(FileIcon *fileIcon);
    void OnProperties(FolderIcon *folderIcon);
    void OnProperties(FileIcon *fileIcon);
    void OnNewFolder(wxCommandEvent &event);
    void OnNewFile(wxCommandEvent &event);
    //------------------------------------------------------------------------------------

    friend class FolderTreeStructurePanel;

private:
    wxDECLARE_EVENT_TABLE();

    wxString formatTime(std::filesystem::file_time_type ftime);
    wxString getPermissions(std::filesystem::perms p) const;
    wxString formatSize(uintmax_t size);
    uintmax_t GetDirectorySize(const wxString &dirPath);

    wxBoxSizer *mainSizer;
    wxBoxSizer *sizer;
    BufferedBitmap *bitmap;
    wxStaticBitmap *bitmapControl;
    wxBoxSizer *sizer_bottom;
    wxStaticText *dimensionsValue;
    wxBoxSizer *propertiesSizer;
    wxBoxSizer *rowSizer;
    wxGridBagSizer *top_grid;
    wxBoxSizer *quickAccessSizer;
    wxBoxSizer *folderStructureSizer;
    FolderTreeStructurePanel *quickAccessPanel;
    wxString path;
    wxSize previousSize;
    bool isMaximized;
    bool isRestored;
    FolderIcon *folderIcon;
    wxString currentPath;
    wxTreeCtrl *m_treeCtrl;
    wxTreeItemId rootId;
    wxScrolledWindow *panel_top;
    wxScrolledWindow *panel_bottom;
    wxScrolledWindow *panel_left;
    wxScrolled<wxPanel> *panel_mid;
    wxScrolledWindow *panel_right;
    wxTextCtrl *nameText;
    wxSlider *slider;
    const int iconSize = FromDIP(32);
    wxColor BorderColor = wxColour(50, 50, 50);

    //----------SearchBar related Methods & Properties--------------
    wxSearchCtrl *searchBar;
    StyledButton *searchButton;
    wxDataViewListCtrl *searchResultList;
    wxDataViewColumn *searchResultColumn;
    wxArrayString allSearchItems;

    void OnItemActivatedAtSearchResult(wxDataViewEvent &event);
    void OnSearch(wxCommandEvent &event);
    void OnSearchButton();
    void OnSize(wxSizeEvent &event);
    void OnMove(wxMoveEvent &event);
    void UpdateSearchResultPosition();
    //--------------------------------------------------------------

    //----------contextMenu Related Properties-----------------------
    bool isCut = false;
    wxString cutBuffer;
    wxString copyBuffer;
    //--------------------------------------------------------------

    std::stack<wxString> backwardStack;
    std::stack<wxString> forwardStack;
    std::unordered_map<wxString, std::chrono::time_point<std::chrono::system_clock>> recentlyAccessedFiles;
    std::vector<wxString> recentlyAccessedFolders;

    // ------------Getting Directory Properties Async -----------------------------------
    wxStaticText *nameLabel;
    wxStaticText *sizeLabel;
    wxStaticText *lastModifiedLabel;
    wxStaticText *permissionsLabel;

    wxStaticText *nameValue;
    wxStaticText *sizeValue;
    wxStaticText *lastModifiedValue;
    wxStaticText *permissionsValue;

    int margin = FromDIP(10);
};

#endif // MYFRAME_H