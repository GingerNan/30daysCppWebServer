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
    : m_loop(loop)
{
    m_acceptor = new Acceptor(m_loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    m_acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete m_acceptor;
}

void Server::newConnection(Socket* serv_sock)
{
    Connection* conn = new Connection(m_loop, serv_sock);
    std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    m_connections[serv_sock->getFd()] = conn;
}

void Server::deleteConnection(Socket* sock)
{
    Connection* conn = m_connections[sock->getFd()];
    m_connections.erase(sock->getFd());
    delete conn;
}