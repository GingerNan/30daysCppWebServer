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
    : mainRector_(loop),
    acceptor_(nullptr)
{
    acceptor_ = new Acceptor(mainRector_);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    threadPool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        subReactors_.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors_[i]);
        threadPool_->add(sub_loop);
    }
}

Server::~Server()
{
    delete acceptor_;
    delete threadPool_;
}

void Server::newConnection(Socket* sock)
{
    if (sock->getFd() != -1)
    {
        int random = sock->getFd() % subReactors_.size();
        Connection* conn = new Connection(subReactors_[random], sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections_[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(int sockfd)
{
    if (sockfd != -1)
    {
        auto it = connections_.find(sockfd);
        if (it != connections_.end())
        {
            Connection* conn = connections_[sockfd];
            connections_.erase(sockfd);
            // close(sockfd);       // 正常
            delete conn;            // 会Segmant fault
        }
    }
}