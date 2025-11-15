#pragma once
#include <string>

class Buffer
{
public:
    Buffer();
    ~Buffer();

    void Append(const char* str, int size);
    ssize_t Size();
    const char* ToStr();
    void Clear();
    void Getline();
    void SetBuf(const char*);
private:
    std::string buf_;
};