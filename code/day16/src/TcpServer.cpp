#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <iostream>

TcpServer::TcpServer()
{
    main_reactor_ = std::make_unique<EventLoop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());

    std::function<void(int)> cb = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1);
    acceptor_->SetNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    threadPool_ = std::make_unique<ThreadPool>(size);
    for (int i = 0; i < size; ++i)
    {
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
        sub_reactor_.push_back(std::move(sub_reactor));
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactor_[i].get());
        threadPool_->Add(sub_loop);
    }
}

TcpServer::~TcpServer()
{
}

void TcpServer::Start()
{
    for (size_t i = 0; i < sub_reactor_.size(); ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactor_[i].get());
        threadPool_->Add(std::move(sub_loop));
    }
    main_reactor_->Loop();
}

RC TcpServer::NewConnection(int fd)
{
    assert(fd != -1);
    uint64_t random = fd % sub_reactor_.size();

    std::unique_ptr<Connection> conn = std::make_unique<Connection>(fd, sub_reactor_[random].get());
    std::function<void(int)> cb = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    //conn->SetOnMessageCallback(on_connect_callback_);
    conn->SetOnRecv(on_recv_);

    connections_[fd] = std::move(conn);
    if (on_connect_)
    {
        on_connect_(connections_[fd].get());
    }
    return RC_SUCCESS;
}

RC TcpServer::DeleteConnection(int fd)
{
    std::cout << "[DeleteConnection] fd=" << fd << std::endl;
    auto it = connections_.find(fd);
    assert(it != connections_.end());
    connections_.erase(fd);
    return RC_SUCCESS;
}

void TcpServer::OnConnect(std::function<void(Connection*)> fn)
{
    on_connect_ = std::move(fn);
}

void TcpServer::OnRecv(std::function<void(Connection*)> fn)
{
    on_recv_ = std::move(fn);
}