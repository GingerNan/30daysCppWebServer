#pragma once
#include "common.h"
#include <string>
#include <memory>

class Buffer
{
public:
    DISALLOW_COPY_AND_MOVE(Buffer);
public:
    Buffer() = default;
    ~Buffer() = default;

    const std::string& buf() const;
    const char* c_str() const;
    void SetBuf(const char* buf);

    void Append(const char* str, int size);
    size_t Size() const;
    void Clear();
private:
    std::string buf_;
};