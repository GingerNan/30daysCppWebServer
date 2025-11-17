#include "Buffer.h"

#include <iostream>
#include <cstring>

void Buffer::Append(const char* str, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (str[i] == '\0') break;
        buf_.push_back(str[i]);
    }
}

size_t Buffer::Size() const
{
    return buf_.size();
}

const char* Buffer::c_str() const
{
    return buf_.c_str();
}

void Buffer::Clear()
{
    buf_.clear();
}

void Buffer::SetBuf(const char* buf)
{
    buf_.clear();
    buf_.append(buf);
}

const std::string& Buffer::buf() const
{
    return buf_;
}