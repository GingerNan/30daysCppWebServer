#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(m_fd == -1, "socket create error");
}

Socket::Socket(int _fd)
    : m_fd(_fd)
{
    errif(m_fd == -1, "socket create error");
}

Socket::~Socket()
{
    if (m_fd != -1)
    {
        close(m_fd);
        m_fd = -1;
    }
}

void Socket::bind(InetAddress* addr)
{
    int ret = ::bind(m_fd, (sockaddr*)&addr->m_addr, addr->m_addr_len);
    errif(ret == -1, "socket bind error");
}
    
void Socket::listen()
{
    int ret = ::listen(m_fd, SOMAXCONN);
    errif(ret == -1, "socket listen error");
}

void Socket::setnonblocking()
{
    fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr)
{
    int client_sockfd = ::accept(m_fd, (sockaddr*)&addr->m_addr, &addr->m_addr_len);
    errif(client_sockfd == -1, "socket accept error");
    return client_sockfd;
}

void Socket::connect(InetAddress* addr)
{
    int ret = ::connect(m_fd, (sockaddr*)&addr->m_addr, addr->m_addr_len);
    errif(ret == -1, "socket connect error");
}

int Socket::getFd()
{
    return m_fd;
}