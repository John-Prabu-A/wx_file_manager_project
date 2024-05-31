#include "FileManager.h"

void FileManager::displayCurrentDirectory()
{
    std::cout << "Current Directory: " << currentPath << std::endl;
}

void FileManager::listFiles()
{
    for (const auto &entry : std::filesystem::directory_iterator(currentPath))
    {
        std::cout << (entry.is_directory() ? "[DIR]  " : "[FILE] ")
                  << entry.path().filename().string() << std::endl;
    }
}

void FileManager::changeDirectory(const std::string &path)
{
    std::filesystem::path newPath = path;
    if (!newPath.is_absolute())
    {
        newPath = currentPath / path;
    }

    if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath))
    {
        backwardStack.push(currentPath);
        currentPath = newPath;
        forwardStack = std::stack<std::filesystem::path>(); // Clear forward stack
        std::cout << "Changed directory to " << currentPath << std::endl;
    }
    else
    {
        std::cout << "Invalid directory: " << newPath << std::endl;
    }
}

void FileManager::displayFileProperties(const std::string &fileName)
{
    std::filesystem::path filePath = currentPath / fileName;
    if (std::filesystem::exists(filePath))
    {
        auto ftime = std::filesystem::last_write_time(filePath);
        auto s = std::filesystem::status(filePath);

        std::cout << "File: " << filePath.filename().string() << std::endl;
        std::cout << "Size: " << formatSize(std::filesystem::file_size(filePath)) << std::endl;
        std::cout << "Last modified: " << formatTime(ftime) << std::endl;
        std::cout << "Permissions: " << getPermissions(s.permissions()) << std::endl;

        // Store recently accessed file
        recentlyAccessedFiles[filePath] = std::chrono::system_clock::now();
    }
    else
    {
        std::cout << "File does not exist." << std::endl;
    }
}
bool FileManager::openFile(const std::string &fileName)
{
    std::filesystem::path filePath = currentPath / fileName;
    if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath))
    {
        std::ifstream fileStream(filePath, std::ios::binary); // Open in binary mode
        if (fileStream)
        {
            // File opened successfully, you can perform operations like reading here
            fileStream.close();
            // Store recently accessed file
            recentlyAccessedFiles[filePath] = std::chrono::system_clock::now();
            recentlyAccessedFolders.push_back(filePath.parent_path().string());
            return true;
        }
        else
        {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "File does not exist or is a directory." << std::endl;
        return false;
    }
}

void FileManager::OpenWithDialog(const std::string &filePath)
{
#ifdef _WIN32
    // Convert std::string to std::wstring
    std::wstring wFilePath(filePath.begin(), filePath.end());

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"openas";
    sei.lpFile = wFilePath.c_str();
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteEx(&sei);
#elif __linux__
    std::string command = "xdg-open \"" + filePath + "\"";
    std::system(command.c_str());
#elif __APPLE__
    std::string command = "open -R \"" + filePath + "\"";
    std::system(command.c_str());
#else
    std::cerr << "Unsupported platform" << std::endl;
#endif

    std::filesystem::path file(filePath);
    recentlyAccessedFiles[file] = std::chrono::system_clock::now();
    recentlyAccessedFolders.push_back(file.parent_path().string());
}

void FileManager::createFile(const std::string &fileName)
{
    std::filesystem::path filePath = currentPath / fileName;

    if (std::filesystem::exists(filePath))
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

    std::ofstream ofs(filePath);
    if (ofs)
    {
        std::cout << "File created: " << filePath << std::endl;
    }
    else
    {
        std::cout << "Failed to create file: " << filePath << std::endl;
    }
}

void FileManager::createDirectory(const std::string &dirName)
{
    std::filesystem::path dirPath = currentPath / dirName;

    if (std::filesystem::exists(dirPath))
    {
        std::cout << "Directory " << dirPath << " already exists." << std::endl;
        return;
    }

    if (std::filesystem::create_directory(dirPath))
    {
        std::cout << "Directory created: " << dirPath << std::endl;
    }
    else
    {
        std::cout << "Failed to create directory: " << dirPath << std::endl;
    }
}

void FileManager::deleteFile(const std::string &fileName)
{
    std::filesystem::path filePath = currentPath / fileName;
    if (std::filesystem::exists(filePath))
    {
        std::filesystem::remove(filePath);
        std::cout << "File deleted: " << filePath << std::endl;
    }
    else
    {
        std::cout << "File does not exist: " << filePath << std::endl;
    }
}

void FileManager::deleteDirectory(const std::string &dirName)
{
    std::filesystem::path dirPath = currentPath / dirName;
    if (std::filesystem::exists(dirPath))
    {
        std::filesystem::remove_all(dirPath);
        std::cout << "Directory deleted: " << dirPath << std::endl;
    }
    else
    {
        std::cout << "Directory does not exist: " << dirPath << std::endl;
    }
}

void FileManager::renameFile(const std::string &oldName, const std::string &newName)
{
    std::filesystem::path oldPath = currentPath / oldName;
    std::filesystem::path newPath = currentPath / newName;
    if (std::filesystem::exists(oldPath))
    {
        std::filesystem::rename(oldPath, newPath);
        std::cout << "File renamed from " << oldPath.filename() << " to " << newPath.filename() << std::endl;
    }
    else
    {
        std::cout << "File does not exist: " << oldPath << std::endl;
    }
}

void FileManager::renameDirectory(const std::string &oldName, const std::string &newName)
{
    std::filesystem::path oldPath = currentPath / oldName;
    std::filesystem::path newPath = currentPath / newName;
    if (std::filesystem::exists(oldPath))
    {
        std::filesystem::rename(oldPath, newPath);
        std::cout << "Directory renamed from " << oldName << " to " << newName << std::endl;
    }
    else
    {
        std::cout << "Directory does not exist: " << oldPath << std::endl;
    }
}

void FileManager::searchFiles(const std::string &query)
{
    std::regex regexQuery(query, std::regex_constants::icase); // Case-insensitive regex
    for (const auto &entry : std::filesystem::recursive_directory_iterator(currentPath))
    {
        if (std::regex_search(entry.path().filename().string(), regexQuery))
        {
            std::cout << (entry.is_directory() ? "[DIR] " : "[FILE] ")
                      << entry.path().string() << std::endl;
        }
    }
}

void FileManager::searchFilesRecursively(const std::string &query)
{
    std::regex regexQuery(query, std::regex_constants::icase); // Case-insensitive regex
    for (const auto &entry : std::filesystem::recursive_directory_iterator(currentPath))
    {
        if (std::regex_search(entry.path().filename().string(), regexQuery))
        {
            std::cout << (entry.is_directory() ? "[DIR] " : "[FILE] ")
                      << entry.path().string() << std::endl;
        }
    }
}

void FileManager::navigateBack()
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

void FileManager::navigateForward()
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

void FileManager::gotoParentDirectory()
{
    std::filesystem::path parentPath = currentPath.parent_path();
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

void FileManager::navigateToHome()
{
    currentPath = std::filesystem::current_path();
    std::cout << "Navigated to home directory: " << currentPath << std::endl;
}

void FileManager::displayRecentlyAccessedFolders()
{
    std::cout << "Recently accessed folders:" << std::endl;
    for (const auto &pair : recentlyAccessedFiles)
    {
        std::filesystem::path folderPath = pair.first;
        if (std::filesystem::is_directory(folderPath))
        {
            auto ftime = std::filesystem::last_write_time(folderPath);
            auto sctp = std::chrono::time_point_cast<std::filesystem::file_time_type::clock::duration>(pair.second);
            std::filesystem::last_write_time(folderPath, std::filesystem::file_time_type{sctp.time_since_epoch()});
            std::cout << folderPath << " - " << formatTime(ftime) << std::endl;
        }
    }
}

void FileManager::displayRecentlyAccessedFiles()
{
    std::cout << "Recently accessed files:" << std::endl;
    for (const auto &pair : recentlyAccessedFiles)
    {
        std::filesystem::path filePath = pair.first;
        auto ftime = std::filesystem::last_write_time(filePath);
        auto sctp = std::chrono::time_point_cast<std::filesystem::file_time_type::clock::duration>(pair.second);
        std::filesystem::last_write_time(filePath, std::filesystem::file_time_type{sctp.time_since_epoch()});
        std::cout << filePath << " - " << formatTime(ftime) << std::endl;
    }
}

bool FileManager::setFilePermissions(const std::string &filename, int permissions)
{
    std::filesystem::path filePath = currentPath / filename;
    if (std::filesystem::exists(filePath))
    {
        try
        {
            std::filesystem::permissions(filePath, std::filesystem::perms(permissions));
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
        std::cerr << "File does not exist: " << filePath << std::endl;
        return false;
    }
}

void FileManager::displayDirectoryProperties(const std::string &dirName)
{
    std::filesystem::path dirPath = currentPath / dirName;
    if (std::filesystem::exists(dirPath))
    {
        auto ftime = std::filesystem::last_write_time(dirPath);
        auto s = std::filesystem::status(dirPath);

        std::cout << "Directory: " << dirPath.filename().string() << std::endl;
        std::cout << "Size: " << formatSize(getDirectorySize(dirPath)) << std::endl;
        std::cout << "Last modified: " << formatTime(ftime) << std::endl;
        std::cout << "Permissions: " << getPermissions(s.permissions()) << std::endl;
    }
    else
    {
        std::cout << "Directory does not exist." << std::endl;
    }
}

// helper functions
std::string FileManager::formatTime(std::filesystem::file_time_type ftime)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&cftime), "%F %T");
    return ss.str();
}

std::string FileManager::FileManager::getPermissions(std::filesystem::perms p)
{
    std::string permStr;
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

std::string FileManager::formatSize(uintmax_t size)
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
    return ss.str();
}

uintmax_t FileManager::getDirectorySize(const std::filesystem::path &dirPath)
{
    if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
    {
        uintmax_t size = 0;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(dirPath))
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
