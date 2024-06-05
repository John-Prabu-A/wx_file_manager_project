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
#include <wx/event.h>

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
#include "Trie.h"
#include "SplayTree.h"

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

    // Navigation functions
    void navigateToHome();
    void gotoParentDirectory();
    void navigateForward();
    void navigateBack();
    void NavigateTo(const wxString &path);
    // void OnMouseEvents(wxMouseEvent &event);

    // File and directory operations
    void renameDirectory(const wxString &oldName, const wxString &newName);
    void renameFile(const wxString &oldName, const wxString &newName);
    void deleteDirectory(const wxString &dirName);
    void deleteFile(const wxString &fileName);
    void createDirectory(const wxString &dirName);
    void createFile(const wxString &fileName);
    void OpenFile(const wxString &filePath);
    std::vector<wxString> GetFilesAndFolders();
    bool setFilePermissions(const wxString &filename, int permissions);
    bool CopyFile(const wxString &sourceFile, const wxString &destFile);
    bool CopyDirectory(const wxString &sourceDir, const wxString &destDir);

    // Properties panel functions
    void HidePropertiesPanel();
    void ShowPropertiesPanel();
    void CreateRightPanel();
    Props GetFileProperties(const wxString &fileName);
    Props GetDirectoryProperties(const wxString &dirName);
    void OnProperties(wxCommandEvent &event);
    void UpdatePropertiesUI(const Props &props);
    void MakePropertiesPanelTextEmpty();

    // Asynchronous properties retrieval
    uintmax_t GetDirectorySize(const std::filesystem::path &dirPath);
    std::future<wxString> GetDirectorySizeAsync(const std::filesystem::path &dirPath);
    std::future<wxString> GetLastWriteTimeAsync(const std::filesystem::path &dirPath);
    std::future<wxString> GetPermissionsAsync(const std::filesystem::path &dirPath);

    // Utility functions
    wxString GetTrashDirectory();
    void PopulateFolderIcons(const wxString &path, wxSizer *sizer);
    void OnFolderPathChange(wxString folderPath);
    void MakeTrie(wxString path);
    wxBitmap ResizeImageToWidth(const wxBitmap &originalBitmap, int newWidth);
    void setPropertiesIcon(const wxString &iconPath);
    wxString formatTime(std::filesystem::file_time_type ftime);
    wxString getPermissions(std::filesystem::perms p) const;
    wxString formatSize(uintmax_t size);

    // Search functionality
    void searchFilesRecursively(const wxString &query);
    void searchFilesRecursively(const std::string &query);
    void OnSearch(wxCommandEvent &event);
    void OnSearchButton();
    void SearchWithTrie();
    void OnItemActivatedAtSearchResult(wxDataViewEvent &event);

    // Context menu functions
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

    // Mouse Events
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);
    void OnRightDown(wxMouseEvent &event);
    void OnRightUp(wxMouseEvent &event);
    void OnMotion(wxMouseEvent &event);
    void OnAux1Down(wxMouseEvent &event);
    void OnAux1Up(wxMouseEvent &event);
    void OnAux2Down(wxMouseEvent &event);
    void OnAux2Up(wxMouseEvent &event);

    friend class FolderTreeStructurePanel;

private:
    wxDECLARE_EVENT_TABLE();

    // UI components
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

    // Search bar components
    wxSearchCtrl *searchBar;
    StyledButton *searchButton;
    wxScrolledWindow *scrolledWindow;
    wxDataViewListCtrl *searchResultList;
    wxDataViewColumn *searchResultColumn;
    wxArrayString allSearchItems;
    Trie trie;

    void OnSize(wxSizeEvent &event);
    void OnMove(wxMoveEvent &event);
    void UpdateSearchResultPosition();

    // Context menu state
    bool isCut = false;
    wxString cutBuffer;
    wxString copyBuffer;

    // Navigation history
    std::stack<wxString> backwardStack;
    std::stack<wxString> forwardStack;

    // Recently accessed files and folders
    SplayTree<wxString> recentlyAccessedFiles;
    std::vector<wxString> recentlyAccessedFolders;

    // Properties panel components
    wxStaticText *nameLabel;
    wxStaticText *sizeLabel;
    wxStaticText *lastModifiedLabel;
    wxStaticText *permissionsLabel;

    wxStaticText *nameValue;
    wxStaticText *sizeValue;
    wxStaticText *lastModifiedValue;
    wxStaticText *permissionsValue;

    int margin = FromDIP(10);
    int RecentLimit = 25;
};

#endif // MYFRAME_H
