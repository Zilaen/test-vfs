#ifndef VIRTUAL_FILE_SYSTEM_H
#define VIRTUAL_FILE_SYSTEM_H

#include <iostream>
#include <cstring>
#include <unordered_set>
#include <mutex>
#include <thread>
#include "File.h"

class VirtualFileSystem {
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    void Create(const char* name, const char* contents);
    File* Open(const char* name) const;
    size_t Write(const char* fileName, const char* data) const;
    size_t Read(const char* fileName) const;
    void Close(const char* fileName);

private:
    std::unordered_set<File*> files;
    mutable std::mutex mutex;
    std::thread fileManagerThread;
    bool running;
    void startFileManagerThread();
};

#endif // VIRTUAL_FILE_SYSTEM_H