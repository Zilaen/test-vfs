#ifndef VIRTUAL_FILE_SYSTEM_H
#define VIRTUAL_FILE_SYSTEM_H

#include <iostream>
#include <cstring>
#include <unordered_set>
#include <mutex>
#include <thread>

class File {
public:
    File(const char* name, const char* contents);
    ~File();

    const char* getName() const;
    size_t write(const char* data);
    size_t read() const;
    void close();

private:
    char* name;
    char* contents;
    mutable std::mutex mutex;
    size_t length;
};

class VirtualFileSystem {
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    void Create(const char* name, const char* contents);
    File* Open(const char* name) const;
    size_t Write(const char* fileName, const char* data) const;
    size_t Read(const char* fileName) const;
    void Close(const char* fileName);
    void startFileManagerThread();

private:
    std::unordered_set<File*> files;
    mutable std::mutex mutex;
    std::thread fileManagerThread;
    bool running;
};

#endif // VIRTUAL_FILE_SYSTEM_H