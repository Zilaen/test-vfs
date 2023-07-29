#include "VirtualFileSystem.h"

File::File(const char* name, const char* contents) {
    this->name = new char[strlen(name) + 1];
    strcpy_s(this->name, strlen(name) + 1, name);
    this->contents = new char[strlen(contents) + 1];
    strcpy_s(this->contents, strlen(contents) + 1, contents);
    length = strlen(contents);
}

File::~File() {
    delete[] name;
    delete[] contents;
}

const char* File::getName() const {
    return name;
}

size_t File::write(const char* data) {
    std::lock_guard<std::mutex> lock(mutex);
    delete[] contents;
    contents = new char[strlen(data) + 1];
    strcpy_s(contents, strlen(data) + 1, data);
    length = strlen(data);
    return length;
}

size_t File::read() const {
    std::lock_guard<std::mutex> lock(mutex);
    return length;
}

void File::close() {
}

VirtualFileSystem::VirtualFileSystem() : running(true) {
    startFileManagerThread(); // start our VFS
}

VirtualFileSystem::~VirtualFileSystem() {
    running = false;
    if (fileManagerThread.joinable()) {
        fileManagerThread.join();
    }
    for (auto& file : files) {
        file->close();
        delete file;
    }
}

void VirtualFileSystem::Create(const char* name, const char* contents) {
    std::lock_guard<std::mutex> lock(mutex);
    files.insert(new File(name, contents));
}

File* VirtualFileSystem::Open(const char* name) const {
    std::lock_guard<std::mutex> lock(mutex);
    for(auto& file : files) {
        if (strcmp(file->getName(), name) == 0) {
            return file;
        }
    }
    return nullptr;
}

size_t VirtualFileSystem::Write(const char* fileName, const char* data) const {
    File* file = Open(fileName);
    if (file) {
        return file->write(data);
    } else {
        std::cout << "File not found: " << fileName << std::endl;
        return 0;
    }
}

size_t VirtualFileSystem::Read(const char* fileName) const {
    File* file = Open(fileName);
    if (file) {
        return file->read();
    } else {
        std::cout << "File not found: " << fileName << std::endl;
        return 0;
    }
}

void VirtualFileSystem::Close(const char* fileName) {
    File* file = Open(fileName);
    if (file) {
        file->close();
        std::lock_guard<std::mutex> lock(mutex);
        files.erase(file);
        delete file;
    } else {
        std::cout << "File not found: " << fileName << std::endl;
    }
}

void VirtualFileSystem::startFileManagerThread() {
    fileManagerThread = std::thread([&]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                std::unique_lock<std::mutex> lock(mutex);
                std::cout << "File Manager Thread is running..." << std::endl;
            }
    });
}

