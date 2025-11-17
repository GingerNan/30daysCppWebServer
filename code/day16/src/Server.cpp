#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "Util.h"
#include "Exception.h"

#include <functional>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop)
    : main_reactor_(loop),
    acceptor_(nullptr),
    threadPool_(nullptr)
{
    acceptor_ = new Acceptor(main_reactor_);
    std::function<void(Socket*)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    threadPool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        sub_reactor_.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactor_[i]);
        threadPool_->Add(sub_loop);
    }
}

Server::~Server()
{
    for (EventLoop* each : sub_reactor_)
    {
        delete each;
    }

    delete acceptor_;
    delete threadPool_;
}

void Server::NewConnection(Socket* sock)
{
    if (sock->GetFd() == -1)
    {
        throw Exception(ExceptionType::INVALID_SOCKET, "New Connection error, invalid cliet socket!");
    }

    int random = sock->GetFd() % sub_reactor_.size();
    Connection* conn = new Connection(sub_reactor_[random], sock);
    std::function<void(Socket*)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    //conn->SetOnMessageCallback(on_connect_callback_);
    conn->SetOnMessageCallback(on_message_callback_);
    connections_[sock->GetFd()] = conn;
    
    if (new_connect_callback_)
    {
        new_connect_callback_(conn);
    }
}

void Server::DeleteConnection(Socket* sock)
{
    int sockfd = sock->GetFd();
    
    auto it = connections_.find(sockfd);
    if (it != connections_.end())
    {
        Connection* conn = connections_[sockfd];
        connections_.erase(sockfd);
        delete conn;
        conn = nullptr;
    }
}

void Server::OnConnect(std::function<void(Connection*)> fn)
{
    on_connect_callback_ = std::move(fn);
}

void Server::OnMessage(std::function<void(Connection*)> fn)
{
    on_message_callback_ = std::move(fn);
}

void Server::NewConnect(std::function<void(Connection*)> fn)
{
    new_connect_callback_ = std::move(fn);
}