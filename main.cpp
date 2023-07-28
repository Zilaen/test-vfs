#include <iostream>
#include <cstring>
#include <unordered_set>
#include <mutex>
#include <thread>

class File {
public:
    File(const char* name, const char* contents) {
        this->name = new char[strlen(name) + 1];
        strcpy_s(this->name, strlen(name) + 1, name);
        this->contents = new char[strlen(contents) + 1];
        strcpy_s(this->contents, strlen(contents) + 1, contents);
        length = strlen(contents);
    }

    ~File() {
        delete[] name;
        delete[] contents;
    }

    const char* getName() const {
        return name;
    }

    size_t write(const char* data) {
        std::lock_guard<std::mutex> lock(mutex);
        delete[] contents;
        contents = new char[strlen(data) + 1];
        strcpy_s(contents, strlen(data) + 1, data);
        length = strlen(data);
        return length;
    }

    size_t read() const {
        std::lock_guard<std::mutex> lock(mutex);
        return length;
    }

    void close() {
    }

private:
    char* name;
    char* contents;
    mutable std::mutex mutex;
    size_t length;
};

class VirtualFileSystem {
public:
    VirtualFileSystem() : running(true) {
        startFileManagerThread(); // start our VFS
    }

    ~VirtualFileSystem() {
        running = false;
        if (fileManagerThread.joinable()) {
            fileManagerThread.join();
        }
        for (auto& file : files) {
            file->close();
            delete file;
        }
    }

    void Create(const char* name, const char* contents) {
        std::lock_guard<std::mutex> lock(mutex);
        files.insert(new File(name, contents));
    }

    File* Open(const char* name) const {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& file : files) {
            if (strcmp(file->getName(), name) == 0) {
                return file;
            }
        }
        return nullptr;
    }

    size_t Write(const char* fileName, const char* data) const {
        File* file = Open(fileName);
        if (file) {
            return file->write(data);
        }
        else {
            std::cout << "File not found: " << fileName << std::endl;
            return 0;
        }
    }

    size_t Read(const char* fileName) const {
        File* file = Open(fileName);
        if (file) {
            return file->read();
        }
        else {
            std::cout << "File not found: " << fileName << std::endl;
            return 0;
        }
    }

    void Close(const char* fileName) {
        File* file = Open(fileName);
        if (file) {
            file->close();
            std::lock_guard<std::mutex> lock(mutex);
            files.erase(file);
            delete file;
        }
        else {
            std::cout << "File not found: " << fileName << std::endl;
        }
    }

    void startFileManagerThread() {
        fileManagerThread = std::thread([&]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                std::unique_lock<std::mutex> lock(mutex);
                std::cout << "File Manager Thread is running..." << std::endl;
            }
        });
    }

private:
    std::unordered_set<File*> files;
    mutable std::mutex mutex;
    std::thread fileManagerThread;
    bool running;
};

int main() {
    VirtualFileSystem vfs;

    std::thread thread1([&]() { // test our VFS
        vfs.Create("GameResources.pak", "Some Game resources.");

        size_t bytesWritten = vfs.Write("GameResources.pak", "Updated Game Resources.");
        std::cout << bytesWritten << " bytes written." << std::endl;

        size_t bytesRead = vfs.Read("GameResources.pak");
        std::cout << bytesRead << " bytes read." << std::endl;

        vfs.Close("GameResources.pak");
    });

    thread1.join();

    std::thread thread2([&]() {
        vfs.Create("Audio/GameAudio.pak", "Some Audio resources. \n");

        size_t bytesWritten = vfs.Write("Audio/GameAudio.pak", "Updated Audio Resources. \n");
        std::cout << bytesWritten << " bytes written." << std::endl;

        size_t bytesRead = vfs.Read("Audio/GameAudio.pak");
        std::cout << bytesRead << " bytes read." << std::endl;

        vfs.Close("Audio/GameAudio.pak");
    });

    thread2.join();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}