#include "Buffer.h"
#include "iostream"

Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

void Buffer::append(const char* str, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (str[i] == '\0') break;
        m_buf.push_back(str[i]);
    }
}

ssize_t Buffer::size()
{
    return m_buf.size();
}

const char* Buffer::c_str()
{
    return m_buf.c_str();
}

void Buffer::clear()
{
    m_buf.clear();
}
 
void Buffer::getline()
{
    m_buf.clear();
    std::getline(std::cin, m_buf);
}