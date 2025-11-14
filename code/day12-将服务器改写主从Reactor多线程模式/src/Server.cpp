#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"

#include <functional>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop)
    : m_loop(loop),
    m_acceptor(nullptr)
{
    m_acceptor = new Acceptor(m_loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    m_acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete m_acceptor;
}

void Server::newConnection(Socket* sock)
{
    if (sock->getFd() != -1)
    {
        Connection* conn = new Connection(m_loop, sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        m_connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(int sockfd)
{
    if (sockfd != -1)
    {
        auto it = m_connections.find(sockfd);
        if (it != m_connections.end())
        {
            Connection* conn = m_connections[sockfd];
            m_connections.erase(sockfd);
            delete conn;
        }
    }
}