#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/datetime.h>
#include <wx/regex.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <stack>
#include <filesystem>

template <typename wxString>
class FileManager
{
private:
    wxString currentPath;
    std::stack<wxString> backwardStack;
    std::stack<wxString> forwardStack;
    std::map<wxString, std::chrono::time_point<std::chrono::system_clock>> recentlyAccessedFiles;
    std::vector<wxString> recentlyAccessedFolders;

    wxString formatTime(std::filesystem::file_time_type ftime) const;
    wxString getPermissions(std::filesystem::perms p) const;
    wxString formatSize(uintmax_t size) const;

public:
    FileManager();
    std::vector<wxString> getFilesAndFolders() const;
    void changeDirectory(const wxString &path);
    bool openFile(const wxString &fileName);
    void createFile(const wxString &fileName);
    void createDirectory(const wxString &dirName);
    void deleteFile(const wxString &fileName);
    void deleteDirectory(const wxString &dirName);
    void renameFile(const wxString &oldName, const wxString &newName);
    void renameDirectory(const wxString &oldName, const wxString &newName);
    void searchFiles(const wxString &query);
    void searchFilesRecursively(const wxString &query);
    void navigateBack();
    void navigateForward();
    void gotoParentDirectory();
    void navigateToHome();
    bool setFilePermissions(const wxString &filename, int permissions);
    void displayDirectoryProperties(const wxString &dirName);
    uintmax_t getDirectorySize(const wxString &dirPath);
};

#endif // FILE_MANAGER_H
