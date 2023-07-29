#ifndef FILE_H
#define FILE_H

#include <cstring>
#include <mutex>

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

#endif // FILE_H