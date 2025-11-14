#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

#include <functional>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop)
    : m_mainRector(loop),
    m_acceptor(nullptr)
{
    m_acceptor = new Acceptor(m_mainRector);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    m_acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    m_threadPool = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        m_subReactors.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, m_subReactors[i]);
        m_threadPool->add(sub_loop);
    }
}

Server::~Server()
{
    delete m_acceptor;
    delete m_threadPool;
}

void Server::newConnection(Socket* sock)
{
    if (sock->getFd() != -1)
    {
        int random = sock->getFd() % m_subReactors.size();
        Connection* conn = new Connection(m_subReactors[random], sock);
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
            // close(sockfd);       // 正常
            delete conn;            // 会Segmant fault
        }
    }
}