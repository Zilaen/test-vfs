#include "File.h"

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
    std::lock_guard<std::mutex> lock(mutex); // TODO: shared_lock
    return length;
}

void File::close() {
}