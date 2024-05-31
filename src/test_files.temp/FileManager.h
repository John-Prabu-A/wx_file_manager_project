#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <regex>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#elif defined(__linux__) || defined(__APPLE__) && defined(__MACH__)
#include <cstdlib>
#endif

class FileManager {
public:
    void displayCurrentDirectory();
    void listFiles();
    void changeDirectory(const std::string& path);
    void displayFileProperties(const std::string& fileName);
    bool openFile(const std::string& fileName);
    void createFile(const std::string& fileName);
    void deleteFile(const std::string& fileName);
    void renameFile(const std::string& oldName, const std::string& newName);
    void searchFiles(const std::string& query);
    void searchFilesRecursively(const std::string& query);
    void navigateBack();
    void navigateForward();
    void navigateToHome();
    void displayRecentlyAccessedFolders();
    void displayRecentlyAccessedFiles();
    bool setFilePermissions(const std::string& filename, int permissions);
    void createDirectory(const std::string& dirname);
    void OpenWithDialog(const std::string& filePath);
    void gotoParentDirectory();
    void renameDirectory(const std::string& oldName, const std::string& newName);
    void deleteDirectory(const std::string& dirname);
    void displayDirectoryProperties(const std::string& dirname);



private:
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::stack<std::filesystem::path> backwardStack;
    std::stack<std::filesystem::path> forwardStack;
    std::vector<std::string> recentlyAccessedFolders;
    std::map<std::filesystem::path, std::chrono::system_clock::time_point> recentlyAccessedFiles;

    std::string formatSize(uintmax_t size);
    std::string formatTime(std::filesystem::file_time_type ftime);
    std::string getPermissions(std::filesystem::perms p);
    uintmax_t getDirectorySize(const std::filesystem::path& dirPath);
};

#endif // FILEMANAGER_H
