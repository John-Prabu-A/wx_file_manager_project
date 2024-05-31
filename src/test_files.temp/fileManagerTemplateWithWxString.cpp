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

    wxString formatTime(std::filesystem::file_time_type ftime) const
    {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        wxStringStream ss;
        ss << std::put_time(std::localtime(&cftime), "%F %T");
        return ss.str();
    }

    wxString getPermissions(std::filesystem::perms p) const
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

    wxString formatSize(uintmax_t size) const
    {
        static const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
        int suffixIndex = 0;
        double fileSize = static_cast<double>(size);

        while (fileSize >= 1024.0 && suffixIndex < sizeof(suffixes) / sizeof(suffixes[0]) - 1)
        {
            fileSize /= 1024.0;
            suffixIndex++;
        }

        wxStringStream ss;
        ss << std::fixed << std::setprecision(2) << fileSize << " " << suffixes[suffixIndex];
        return ss.str();
    }

public:
    FileManager()
    {
        currentPath = wxStandardPaths::Get().GetUserDir();
    }

    void displayCurrentDirectory() const
    {
        std::cout << "Current Directory: " << currentPath << std::endl;
    }

    void listFiles() const
    {
        wxDir dir(currentPath);
        wxString filename;
        bool cont = dir.GetFirst(&filename);
        while (cont)
        {
            std::cout << (wxFileName::DirExists(currentPath + wxFileName::GetPathSeparator() + filename)
                              ? "[DIR]  "
                              : "[FILE] ")
                      << filename << std::endl;
            cont = dir.GetNext(&filename);
        }
    }

    std::vector<wxString> getFilesAndFolders() const
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

    void changeDirectory(const wxString &path)
    {
        wxString newPath = path;
        if (!wxFileName::IsPathSeparator(path[path.size() - 1]))
        {
            newPath = currentPath + wxFileName::GetPathSeparator() + path;
        }

        if (wxFileName::DirExists(newPath))
        {
            backwardStack.push(currentPath);
            currentPath = newPath;
            forwardStack = std::stack<wxString>(); // Clear forward stack
            std::cout << "Changed directory to " << currentPath << std::endl;
        }
        else
        {
            std::cout << "Invalid directory: " << newPath << std::endl;
        }
    }

    void displayFileProperties(const wxString &fileName) const
    {
        wxString filePath = currentPath + wxFileName::GetPathSeparator() + fileName;
        if (wxFileExists(filePath))
        {
            wxDateTime ftime = wxFileModificationTime(filePath);
            wxString perm = getPermissions(std::filesystem::status(filePath.ToStdString()).permissions());
            std::cout << "File: " << fileName << std::endl;
            std::cout << "Size: " << formatSize(wxFileSize(filePath)) << std::endl;
            std::cout << "Last modified: " << ftime.Format("%F %T") << std::endl;
            std::cout << "Permissions: " << perm << std::endl;

            recentlyAccessedFiles[filePath] = std::chrono::system_clock::now();
        }
        else
        {
            std::cout << "File does not exist." << std::endl;
        }
    }

    bool openFile(const wxString &fileName)
    {
        wxString filePath = currentPath + wxFileName::GetPathSeparator() + fileName;
        if (wxFileExists(filePath))
        {
            wxFile file(filePath);
            if (file.IsOpened())
            {
                file.Close();
                recentlyAccessedFiles[filePath] = std::chrono::system_clock::now();
                recentlyAccessedFolders.push_back(wxFileName(filePath).GetPath());
                return true;
            }
            else
            {
                std::cerr << "Failed to open file: " << fileName << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "File does not exist: " << fileName << std::endl;
            return false;
        }
    }

    void createFile(const wxString &fileName)
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

    void createDirectory(const wxString &dirName)
    {
        wxString dirPath = currentPath + wxFileName::GetPathSeparator() + dirName;

        if (wxDirExists(dirPath))
        {
            std::filesystem::path newPath = dirPath.toStdstring();
            if (!newPath.is_absolute())
            {
                newPath = currentPath / path;
            }

            backwardStack.push(currentPath);
            currentPath = newPath;
            forwardStack = std::stack<std::filesystem::path>();
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

    void deleteFile(const wxString &fileName)
    {
        wxString filePath = currentPath + wxFileName::GetPathSeparator() + fileName;
        if (wxFileExists(filePath))
        {
            wxRemoveFile(filePath);
            std::cout << "File deleted: " << fileName << std::endl;
        }
        else
        {
            std::cout << "File does not exist: " << fileName << std::endl;
        }
    }

    void deleteDirectory(const wxString &dirName)
    {
        wxString dirPath = currentPath + wxFileName::GetPathSeparator() + dirName;
        if (wxDirExists(dirPath))
        {
            wxRmdir(dirPath);
            std::cout << "Directory deleted: " << dirName << std::endl;
        }
        else
        {
            std::cout << "Directory does not exist: " << dirName << std::endl;
        }
    }

    void renameFile(const wxString &oldName, const wxString &newName)
    {
        wxString oldPath = currentPath + wxFileName::GetPathSeparator() + oldName;
        wxString newPath = currentPath + wxFileName::GetPathSeparator() + newName;
        if (wxFileExists(oldPath))
        {
            wxRenameFile(oldPath, newPath);
            std::cout << "File renamed from " << oldName << " to " << newName << std::endl;
        }
        else
        {
            std::cout << "File does not exist: " << oldName << std::endl;
        }
    }

    void renameDirectory(const wxString &oldName, const wxString &newName)
    {
        wxString oldPath = currentPath + wxFileName::GetPathSeparator() + oldName;
        wxString newPath = currentPath + wxFileName::GetPathSeparator() + newName;
        if (wxDirExists(oldPath))
        {
            wxRename(oldPath, newPath);
            std::cout << "Directory renamed from " << oldName << " to " << newName << std::endl;
        }
        else
        {
            std::cout << "Directory does not exist: " << oldName << std::endl;
        }
    }

    void searchFiles(const wxString &query)
    {
        wxString regexQuery(query, wxRE_ICASE); // Case-insensitive regex
        wxString filename;
        wxDir dir(currentPath);
        bool cont = dir.GetFirst(&filename, wxString(), wxDIR_FILES | wxDIR_DIRS);
        while (cont)
        {
            if (wxString(filename).Matches(regexQuery))
            {
                std::cout << (wxDirExists(currentPath + wxFileName::GetPathSeparator() + filename)
                                  ? "[DIR] "
                                  : "[FILE] ")
                          << filename << std::endl;
            }
            cont = dir.GetNext(&filename);
        }
    }

    void searchFilesRecursively(const wxString &query)
    {
        wxString regexQuery(query, wxRE_ICASE); // Case-insensitive regex
        wxString filename;
        wxDir dir(currentPath);
        bool cont = dir.GetFirst(&filename, wxString(), wxDIR_FILES | wxDIR_DIRS | wxDIR_DOTDOT);
        while (cont)
        {
            if (wxString(filename).Matches(regexQuery))
            {
                std::cout << (wxDirExists(currentPath + wxFileName::GetPathSeparator() + filename)
                                  ? "[DIR] "
                                  : "[FILE] ")
                          << filename << std::endl;
            }
            cont = dir.GetNext(&filename);
        }
    }

    void navigateBack()
    {
        if (!backwardStack.empty())
        {
            forwardStack.push(currentPath);
            currentPath = backwardStack.top();
            backwardStack.pop();
            std::cout << "Navigated back to " << currentPath << std::endl;
        }
        else
        {
            std::cout << "No backward history available." << std::endl;
        }
    }

    void navigateForward()
    {
        if (!forwardStack.empty())
        {
            backwardStack.push(currentPath);
            currentPath = forwardStack.top();
            forwardStack.pop();
            std::cout << "Navigated forward to " << currentPath << std::endl;
        }
        else
        {
            std::cout << "No forward history available." << std::endl;
        }
    }

    void gotoParentDirectory()
    {
        wxString parentPath = wxFileName(currentPath).GetPath();
        if (parentPath != currentPath)
        {
            backwardStack.push(currentPath);
            currentPath = parentPath;
            std::cout << "Navigated to parent directory: " << currentPath << std::endl;
        }
        else
        {
            std::cout << "Already at the root directory." << std::endl;
        }
    }

    void navigateToHome()
    {
        currentPath = wxStandardPaths::Get().GetUserDir();
        std::cout << "Navigated to home directory: " << currentPath << std::endl;
    }

    void displayRecentlyAccessedFolders()
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

    void displayRecentlyAccessedFiles()
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

    bool setFilePermissions(const wxString &filename, int permissions)
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

    void displayDirectoryProperties(const wxString &dirName)
    {
        wxString dirPath = currentPath + wxFileName::GetPathSeparator() + dirName;
        if (wxDirExists(dirPath))
        {
            auto ftime = std::filesystem::last_write_time(dirPath.ToStdString());
            auto s = std::filesystem::status(dirPath.ToStdString());

            std::cout << "Directory: " << dirName << std::endl;
            std::cout << "Size: " << formatSize(getDirectorySize(dirPath.ToStdString())) << std::endl;
            std::cout << "Last modified: " << formatTime(ftime) << std::endl;
            std::cout << "Permissions: " << getPermissions(s.permissions()) << std::endl;
        }
        else
        {
            std::cout << "Directory does not exist." << std::endl;
        }
    }

    uintmax_t getDirectorySize(const wxString &dirPath)
    {
        if (wxDirExists(dirPath))
        {
            uintmax_t size = 0;
            wxDir dir(dirPath);
            wxString filename;
            bool cont = dir.GetFirst(&filename, wxString(), wxDIR_FILES);
            while (cont)
            {
                size += wxFileSize(dirPath + wxFileName::GetPathSeparator() + filename);
                cont = dir.GetNext(&filename);
            }
            return size;
        }
        std::cerr << "Directory does not exist: " << dirPath << std::endl;
        return 0;
    }
};
