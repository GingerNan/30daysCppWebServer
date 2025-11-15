#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "util.h"

#include <functional>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop)
    : mainRector_(loop),
    acceptor_(nullptr)
{
    acceptor_ = new Acceptor(mainRector_);
    std::function<void(Socket*)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    threadPool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        subReactors_.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, subReactors_[i]);
        threadPool_->Add(sub_loop);
    }
}

Server::~Server()
{
    delete acceptor_;
    delete threadPool_;
}

void Server::NewConnection(Socket* sock)
{
    ErrorIf(sock->GetFd() == -1, "new connection error");

    int random = sock->GetFd() % subReactors_.size();
    Connection* conn = new Connection(subReactors_[random], sock);
    std::function<void(Socket*)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    conn->SetOnConnectCallback(on_connect_callback_);
    connections_[sock->GetFd()] = conn;
}

void Server::DeleteConnection(Socket* sock)
{
    int sockfd = sock->GetFd();
    
    auto it = connections_.find(sockfd);
    if (it != connections_.end())
    {
        Connection* conn = connections_[sockfd];
        connections_.erase(sockfd);
        // close(sockfd);       // 正常
        delete conn;            // 会Segmant fault
    }
}

void Server::OnConnect(std::function<void(Connection*)> fn)
{
    on_connect_callback_ = std::move(fn);
}