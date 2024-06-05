# wx_file_manager_project

A multiplatform file manager application using wxWidgets | CMake

## How this works

The main CMake file searches for the wxWidgets library using `FindPackage`. If not found, it downloads the library source from GitHub, compiles it, and links it with the main project.

The super build pattern with `ExternalProject_Add` is used to achieve this.

# Project Contents

## src

- [BufferedBitmap.cpp](src/BufferedBitmap.cpp)
- [BufferedBitmap.h](src/BufferedBitmap.h)
- [CMakeLists.txt](src/CMakeLists.txt)
- [FileIcon.cpp](src/FileIcon.cpp)
- [FileIcon.h](src/FileIcon.h)
- [FolderIcon.cpp](src/FolderIcon.cpp)
- [FolderIcon.h](src/FolderIcon.h)
- [Info.plist](src/Info.plist)
- [FileManager.cpp](src/MyFrame.cpp)
- [FileManager.h](src/MyFrame.h)
- [SplayTree.cpp](src/SplayTree.cpp)
- [SplayTree.h](src/SplayTree.h)
- [StyledButton.cpp](src/StyledButton.cpp)
- [StyledButton.h](src/StyledButton.h)
- [Trie.cpp](src/Trie.cpp)
- [Trie.h](src/Trie.h)
- [TrieNode.cpp](src/TrieNode.cpp)
- [TrieNode.h](src/TrieNode.h)
- [folderTreeStructure.cpp](src/folderTreeStructure.cpp)
- [folderTreeStructure.h](src/folderTreeStructure.h)
- [main.cpp](src/main.cpp)
- [main.exe.manifest](src/main.exe.manifest)

## Requirements

This works on Windows, Mac, and Linux. You'll need `cmake` and a C++ compiler (tested on `clang`, `gcc`, and MSVC).

Linux builds require the GTK3 library and headers installed in the system.

## Building

To build the project, use:

```bash
cmake -S. -Bbuild
cmake --build build
```

This will create a directory named `build` and create all build artifacts there. The main executable can be found in the `build/subprojects/Build/wx_file_manager_project_core` folder.

<br>

# FileManager Project Documentation

## Overview

This documentation provides an in-depth overview of the `FileManager`, which serves as the main frame for a wxWidgets-based file explorer application. The `FileManager` encapsulates various functionalities to navigate the filesystem, manipulate files and directories, display file properties, and manage a search interface. This document will cover essential data structures used, key methods, and the overall architecture of the `FileManager`.

## Table of Contents
1. [Introduction](#introduction)
2. [Problems Faced](#problems-faced)
2. [Key Data Structures](#key-data-structures)
3. [Important Methods](#important-methods)
4. [Search Functionality](#search-functionality)
5. [Properties Panel](#properties-panel)
6. [Context Menu Operations](#context-menu-operations)
7. [Navigation Management](#navigation-management)
8. [Summary](#summary)

## Introduction

The `FileManager` provides a comprehensive interface for file management. It includes:
- Navigation functions for filesystem traversal.
- File and directory operations like creation, deletion, and renaming.
- A properties panel to display metadata of selected files or directories.
- A search functionality with trie-based indexing for efficient querying.
- Context menu operations for user interactions.
- Navigation history tracking for user convenience.

## Problems Faced

### Search Functionality Performance
- **Initial Implementation**: The initial search functionality was implemented using a recursive procedure. This approach caused significant lag and made the UI slow and unresponsive.
- **Solution We Come Up With**: To address this issue, a Trie data structure was implemented for faster access and retrieval. By using a Trie, the search operations became efficient, allowing for quick prefix-based searches.
- **Threading**: The Trie creation process was moved to a separate thread to avoid affecting the main thread. This ensured that the search functionality was responsive and did not interfere with the user experience.

### Recently Accessed Files Management
- **Initial Implementation**: The initial implementation used a vector container in C++ to store and retrieve recently accessed files. As the size of the recently accessed files list grew, the performance degraded, causing slowdowns in the UI.
- **Solution We Come Up With**: To improve performance, a Splay Tree was introduced for managing recently accessed files. The Splay Tree allowed for faster retrieval of recently accessed nodes, making the UI more efficient and responsive.

By addressing these issues with appropriate data structures and threading, the overall performance and user experience of the `FileManager` were significantly improved.

## Key Data Structures

### Trie

#### Description
A Trie (prefix tree) is used for efficient search operations. It allows fast retrieval of files and folders by their prefixes, making it suitable for implementing the search functionality.

#### Efficiency
- **Insertion and Search Time Complexity:** O(n), where n is the length of the word.
- **Space Complexity:** O(ALPHABET_SIZE * n * m), where n is the number of keys and m is the average length of the key.

#### Code Snippet
```cpp
#include "Trie.h"

Trie::Trie()
{
    root = new TrieNode();
}

Trie::~Trie()
{
    delete root;
}

void Trie::insert(const std::string &word)
{
    TrieNode *node = root;
    for (char c : word)
    {
        if (node->children.find(c) == node->children.end())
        {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    node->isEndOfWord = true;
}

bool Trie::search(const std::string &word)
{
    TrieNode *node = root;
    for (char c : word)
    {
        if (node->children.find(c) == node->children.end())
        {
            return false;
        }
        node = node->children[c];
    }
    return node->isEndOfWord;
}

std::vector<std::string> Trie::searchWithPrefix(const std::string &prefix)
{
    std::vector<std::string> results;
    TrieNode *node = root;
    for (char c : prefix)
    {
        if (node->children.find(c) == node->children.end())
        {
            return results;
        }
        node = node->children[c];
    }
    searchWithPrefixHelper(node, prefix, results);
    return results;
}

void Trie::searchWithPrefixHelper(TrieNode *node, std::string prefix, std::vector<std::string> &results)
{
    if (node->isEndOfWord)
    {
        results.push_back(prefix);
    }
    for (const auto &pair : node->children)
    {
        searchWithPrefixHelper(pair.second, prefix + pair.first, results);
    }
}
```

### Splay Tree

#### Description
A Splay Tree is a self-adjusting binary search tree with the additional property that recently accessed elements are quick to access again. It is used for maintaining a list of recently accessed files, ensuring that the most frequently accessed files are quicker to access.

#### Efficiency
- **Average Time Complexity for Search, Insert, Delete:** O(log n)
- **Worst-case Time Complexity for Search, Insert, Delete:** O(n)

#### Code Snippet
```cpp
#include "SplayTree.h"

template <typename T>
SplayTree<T>::SplayTree() : root(nullptr) {}

template <typename T>
SplayTree<T>::~SplayTree()
{
    destroy(root);
}

template <typename T>
void SplayTree<T>::destroy(SplayTreeNode<T> *node)
{
    if (node)
    {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::rightRotate(SplayTreeNode<T> *x)
{
    SplayTreeNode<T> *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::leftRotate(SplayTreeNode<T> *x)
{
    SplayTreeNode<T> *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::splay(SplayTreeNode<T> *root, const T &key)
{
    if (root == nullptr || root->key == key)
        return root;

    if (root->key > key)
    {
        if (root->left == nullptr)
            return root;

        if (root->left->key > key)
        {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        }
        else if (root->left->key < key)
        {
            root->left->right = splay(root->left->right, key);
            if (root->left->right != nullptr)
                root->left = leftRotate(root->left);
        }
        return (root->left == nullptr) ? root : rightRotate(root);
    }
    else
    {
        if (root->right == nullptr)
            return root;

        if (root->right->key > key)
        {
            root->right->left = splay(root->right->left, key);
            if (root->right->left != nullptr)
                root->right = rightRotate(root->right);
        }
        else if (root->right->key < key)
        {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }
        return (root->right == nullptr) ? root : leftRotate(root);
    }
}

template <typename T>
void SplayTree<T>::insert(const T &key)
{
    if (root == nullptr)
    {
        root = new SplayTreeNode<T>(key);
        return;
    }

    root = splay(root, key);

    if (root->key == key)
        return;

    SplayTreeNode<T> *newNode = new SplayTreeNode<T>(key);

    if (root->key > key)
    {
        newNode->right = root;
        newNode->left = root->left;
        root->left = nullptr;
    }
    else
    {
        newNode->left = root;
        newNode->right = root->right;
        root->right = nullptr;
    }

    root = newNode;
}

template <typename T>
bool SplayTree<T>::find(const T &key)
{
    root = splay(root, key);
    return root != nullptr && root->key == key;
}

template <typename T>
void SplayTree<T>::remove(const T &key)
{
    if (root == nullptr)
        return;

    root = splay(root, key);

    if (root->key != key)
        return;

    if (root->left == nullptr)
    {
        SplayTreeNode<T> *temp = root;
        root = root->right;
        delete temp;
    }
    else
    {
        SplayTreeNode<T> *temp = root;
        root = splay(root->left, key);
        root->right = temp->right;
        delete temp;
    }
}

template <typename T>
void SplayTree<T>::inorder(SplayTreeNode<T> *root, std::vector<T> &result)
{
    if (root)
    {
        inorder(root->left, result);
        result.push_back(root->key);
        inorder(root->right, result);
    }
}

template <typename T>
std::vector<T> SplayTree<T>::getRecentlyAccessed()
{
    std::vector<T> result;
    inorder(root, result);
    return result;
}
```

## Important Methods

### Navigation Methods
- `void navigateToHome()`: Navigate to the home directory.
- `void gotoParentDirectory()`: Navigate to the parent directory.
- `void navigateForward()`: Navigate forward in the history.
- `void navigateBack()`: Navigate back in the history.
- `void NavigateTo(const wxString &path)`: Navigate to a specific path.

### File and Directory Operations
- `void renameDirectory(const wxString &oldName, const wxString &newName)`: Rename a directory.
- `void renameFile(const wxString &oldName, const wxString &newName)`: Rename a file.
- `void deleteDirectory(const wxString &dirName)`: Delete a directory.
- `void deleteFile(const wxString &fileName)`: Delete a file.
- `void createDirectory(const wxString &dirName)`: Create a new directory.
- `void createFile(const wxString &fileName)`: Create a new file.
- `void OpenFile(const wxString &filePath)`: Open a file.
- `bool setFilePermissions(const wxString &filename, int permissions)`: Set file permissions.
- `bool CopyFile(const wxString &sourceFile, const wxString &destFile)`: Copy a file.
- `bool CopyDirectory(const wxString &sourceDir, const wxString &destDir)`: Copy a directory.

## Search Functionality

The search functionality leverages the Trie data structure for efficient querying. The search process involves inserting all filenames and directory names into the Trie, which allows for quick prefix-based searches.

### Search Methods
- `void SearchWithTrie()`: Search using the Trie data structure.
```cpp
void MyFrame::SearchWithTrie()
{
    wxString prefix = searchBar->GetValue();
    if (prefix != "")
    {
        searchResultList->DeleteAllItems();
        std::vector<std::string> resultFiles = trie.searchWithPrefix((wxString("[FILE] ") + currentPath + wxFileName::GetPathSeparator() + searchBar->GetValue()).ToStdString());
        std::vector<std::string> resultFolders = trie.searchWithPrefix((wxString("[DIR] ") + currentPath + wxFileName::GetPathSeparator() + searchBar->GetValue()).ToStdString());
        std::vector<std::string> results(resultFiles);
        results.insert(results.end(), resultFolders.begin(), resultFolders.end());
        for (const auto &item : allSearchItems)
        {
            if (item.Lower().Contains(prefix.Lower()))
            {
                wxVector<wxVariant> data;
                data.push_back(wxVariant(item));
                searchResultList->AppendItem(data);
            }
        }
        searchResultList->Show();
    }
    else
    {
        searchResultList->Hide();
    }
    UpdateSearchResultPosition();
}
```
- `void MyFrame::MakeTrie(wxString path)`: Createing the trie for a folder when it is opened asyncronously to the main thread.

```cpp
void MyFrame::MakeTrie(wxString path)
{
    if (!wxDirExists(path))
    {
        wxLogError("The directory does not exist: %s", path);
        return;
    }
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
        wxString fullPath = ((wxDirExists(path + wxFileName::GetPathSeparator() + filename) ? wxString("[DIR] ") : wxString("[FILE] "))) + path + wxFileName::GetPathSeparator() + filename;
        trie.insert(std::string(fullPath.mb_str()));
        std::cout << "Inserting: " << fullPath << std::endl;
        cont = dir.GetNext(&filename);
    }
}
```

- `void OnItemActivatedAtSearchResult(wxDataViewEvent &event)`: Event handler for activating a search result item.
- `void searchFilesRecursively(const wxString &query)`: Search files recursively based on a query.
- `void OnSearch(wxCommandEvent &event)`: Event handler for search input.
- `void OnSearchButton()`: Triggered when the search button is clicked.

## Properties Panel

The properties panel displays metadata for selected files or directories, including name, size, last modified date, and permissions.

### Properties Panel Methods
- `void HidePropertiesPanel()`: Hide the properties panel.
- `void ShowPropertiesPanel()`: Show the properties panel.
- `void CreateRightPanel()`: Create the right panel for displaying properties.
- `Props GetFileProperties(const wxString &fileName)`: Get properties of a file.
- `Props GetDirectoryProperties(const wxString &dirName)`: Get properties of a directory.
- `void OnProperties(wxCommandEvent &event)`: Event handler for properties display.
- `void UpdatePropertiesUI(const Props &props)`: Update the properties panel UI.
- `void MakePropertiesPanelTextEmpty()`: Clear the properties panel text.

## Context Menu Operations

The context menu allows users to perform various actions on files and directories, such as opening, cutting, copying, pasting, deleting, renaming, and viewing properties.

### Context Menu Methods
- `void OnShowContextMenu(wxContextMenuEvent &event)`: Show the context menu.
- `void OnOpen(FolderIcon *folderIcon)`: Open a folder.
- `void OnOpen(FileIcon *fileIcon)`: Open a file.
- `void OnCut(FolderIcon *folderIcon)`: Cut a folder.
- `void OnCut(FileIcon *fileIcon)`: Cut a file.
- `void OnCopy(FolderIcon *folderIcon)`: Copy a folder.
- `void OnCopy(FileIcon *fileIcon)`: Copy a file.
- `void OnPaste(wxString folderPath)`: Paste the clipboard contents.
- `void OnDelete(FolderIcon *folderIcon)`: Delete a folder.
- `void OnDelete(FileIcon *fileIcon)`: Delete a file.
- `void OnRename(FolderIcon *folderIcon)`: Rename a folder.
- `void OnRename(FileIcon *fileIcon)`: Rename a file.
- `void OnProperties(FolderIcon *folderIcon)`: View properties of a folder.
- `void OnProperties(FileIcon *fileIcon)`: View properties of a file.
- `void OnNewFolder(wxCommandEvent &event)`: Create a new folder.
- `void OnNewFile(wxCommandEvent &event)`: Create a new file.

## Navigation Management

The class handles navigation history with two stacks: `backwardStack` and `forwardStack` (using the built-in stack container in C++).. These stacks store paths for backward and forward navigation, allowing the user to traverse their navigation history easily. 

### Navigation History Methods
- `void navigateToHome()`: Navigate to the home directory.
- `void gotoParentDirectory()`: Navigate to the parent directory.
- `void navigateForward()`: Navigate forward in the history.
- `void navigateBack()`: Navigate back in the history.
- `void NavigateTo(const wxString &path)`: Navigate to a specific path.

## Summary

This `FileManager` provides a robust framework for a file explorer application, leveraging efficient data structures like Trie and Splay Tree for search and access management. It encapsulates functionalities for navigation, file operations, properties display, and context menu interactions, offering a comprehensive and user-friendly interface for file management.

By utilizing the Trie data structure, the search functionality is optimized for quick prefix-based queries, while the Splay Tree ensures efficient access to recently accessed files. The integration of wxWidgets components allows for a seamless and responsive user experience, making the `FileManager` a powerful tool for filesystem exploration and manipulation.