#pragma once
#include "common.h"
#include <string>
#include <cstdint>

class Socket
{
public:
    DISALLOW_COPY_AND_MOVE(Socket);
public:
    Socket();
    ~Socket();

    void SetFd(int fd);
    int GetFd() const;

    std::string GetAddr() const;

    RC Create();
    RC Bind(const char* ip, uint16_t port) const;
    RC Listen() const;
    RC Accept(int& client_fd) const;
    RC Connect(const char* ip, uint16_t port) const;

    RC Setnonblocking() const;
    bool IsNonBlocking() const;
    
    size_t RecvBufSize() const;

private:
    int fd_;
};