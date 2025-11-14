#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket()
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd_ == -1, "socket create error");
}

Socket::Socket(int _fd)
    : fd_(_fd)
{
    errif(fd_ == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd_ != -1)
    {
        close(fd_);
        fd_ = -1;
    }
}

void Socket::bind(InetAddress* addr)
{
    int ret = ::bind(fd_, (sockaddr*)&addr->addr_, addr->addr_len_);
    errif(ret == -1, "socket bind error");
}
    
void Socket::listen()
{
    int ret = ::listen(fd_, SOMAXCONN);
    errif(ret == -1, "socket listen error");
}

void Socket::setnonblocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr)
{
    int client_sockfd = ::accept(fd_, (sockaddr*)&addr->addr_, &addr->addr_len_);
    errif(client_sockfd == -1, "socket accept error");
    return client_sockfd;
}

void Socket::connect(InetAddress* addr)
{
    int ret = ::connect(fd_, (sockaddr*)&addr->addr_, addr->addr_len_);
    errif(ret == -1, "socket connect error");
}

int Socket::getFd()
{
    return fd_;
}