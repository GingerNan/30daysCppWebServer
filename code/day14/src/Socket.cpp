#include "Socket.h"
#include "util.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <cstring>

InetAddress::InetAddress()
{
    bzero(&addr_, sizeof addr_);
}

InetAddress::InetAddress(const char* ip, uint16_t port)
{
    bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip);
    addr_.sin_port = htons(port);
}

InetAddress::~InetAddress()
{
}

void InetAddress::SetAddr(sockaddr_in addr)
{
    addr_ = addr;
}

sockaddr_in InetAddress::GetAddr()
{
    return addr_;
}

const char* InetAddress::GetIp()
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::GetPort()
{
    return htons(addr_.sin_port);
}


Socket::Socket()
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrorIf(fd_ == -1, "socket create error");
}

Socket::Socket(int _fd)
    : fd_(_fd)
{
    ErrorIf(fd_ == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd_ != -1)
    {
        close(fd_);
        fd_ = -1;
    }
}

void Socket::Bind(InetAddress* addr)
{
    struct sockaddr_in tmp_addr = addr->GetAddr();
    int ret = ::bind(fd_, (sockaddr*)&tmp_addr, sizeof tmp_addr);
    ErrorIf(ret == -1, "socket bind error");
}
    
void Socket::Listen()
{
    int ret = ::listen(fd_, SOMAXCONN);
    ErrorIf(ret == -1, "socket listen error");
}

void Socket::Setnonblocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

bool Socket::IsNonBlocking()
{
    return fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::Accept(InetAddress* addr)
{
    // for server socket
    int client_sockfd = -1;
    struct sockaddr_in tmp_addr {};
    socklen_t addr_len = sizeof tmp_addr;
    if (IsNonBlocking())
    {
        while (true)
        {
            client_sockfd = ::accept(fd_, (sockaddr*)&tmp_addr, &addr_len);
            if (client_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            {
                //printf("no connection yet\n");
                continue;
            }

            if (client_sockfd == -1)
            {
                ErrorIf(true, "socket accept error");
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        client_sockfd = ::accept(fd_, (sockaddr*)&tmp_addr, &addr_len);
        ErrorIf(client_sockfd == -1, "socket accept error");
    }

    addr->SetAddr(tmp_addr);
    return client_sockfd;
}

void Socket::Connect(InetAddress* addr)
{
    int ret = ::connect(fd_, (sockaddr*)&addr->addr_, addr->addr_len_);
    ErrorIf(ret == -1, "socket connect error");
}

void Socket::Connect(const char* ip, uint16_t port)
{

}

int Socket::GetFd()
{
    return fd_;
}