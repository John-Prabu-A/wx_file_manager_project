# wx_file_manager_project

A multiplatform file manager application using wxWidgets | CMake

## How this works

The main cmake file searches for the wxWidgets library using `FindPackage`. If not found, it downloads the library source from GitHub, compiles it, and links it with the main project. 

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
- [MyFrame.cpp](src/MyFrame.cpp)
- [MyFrame.h](src/MyFrame.h)
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

### resource

- [app_logo.ico](src/resource/app_logo.ico)
- [app_logo.png](src/resource/app_logo.png)
- [default_icon.ico](src/resource/default_icon.ico)
- [default_icon.png](src/resource/default_icon.png)
- [folder_icon.ico](src/resource/folder_icon.ico)
- [folder_icon.png](src/resource/folder_icon.png)
- [folder_icon.psd](src/resource/folder_icon.psd)
- [folder_icon_active.ico](src/resource/folder_icon_active.ico)
- [folder_icon_active.png](src/resource/folder_icon_active.png)
- [folder_icon_normal.ico](src/resource/folder_icon_normal.ico)
- [folder_icon_normal.png](src/resource/folder_icon_normal.png)
- [folder_icon_normal.psd](src/resource/folder_icon_normal.psd)
- [folder_view.png](src/resource/folder_view.png)
- [text_icon.ico](src/resource/text_icon.ico)
- [text_icon.png](src/resource/text_icon.png)

### test_files

- [FileManager.cpp](src/test_files/FileManager.cpp)
- [FileManager.h](src/test_files/FileManager.h)
- [Tries.cpp](src/test_files/Tries.cpp)
- [Tries.h](src/test_files/Tries.h)
- [comboBoxLogic.cpp](src/test_files/comboBoxLogic.cpp)
- [fileManagerTemplateWithWxString.cpp](src/test_files/fileManagerTemplateWithWxString.cpp)
- [fileManagerTemplateWithWxString.h](src/test_files/fileManagerTemplateWithWxString.h)
- [old_MyFrame.cpp](src/test_files/old_MyFrame.cpp)
- [test](src/test_files/test)
- [test.cpp](src/test_files/test.cpp)

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

