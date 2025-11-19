#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"

#include <fcntl.h>
#include <utility>
#include <iostream>
    
Acceptor::Acceptor(EventLoop* loop)
{
    socket_ = std::make_unique<Socket>();
    assert(socket_->Create() == RC_SUCCESS);
    assert(socket_->Bind("127.0.0.1", 8888) == RC_SUCCESS);
    assert(socket_->Listen() == RC_SUCCESS);

    channe_ = std::make_unique<Channel>(socket_->GetFd(), loop);
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    channe_->SetReadCallback(cb);
    channe_->EnbleRead();
}

Acceptor::~Acceptor()
{
}

RC Acceptor::AcceptConnection() const
{
    int client_fd = -1;
    if (socket_->Accept(client_fd) != RC_SUCCESS)
    {
        return RC_ACCEPTOR_ERROR;
    }

    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK); //新接收的连接设置为非阻塞
    if (new_connection_callback_)
    {
        new_connection_callback_(client_fd);
    }
    return RC_SUCCESS;
}

void Acceptor::SetNewConnectionCallback(std::function<void(int)> const& callback)
{
    new_connection_callback_ = std::move(callback);
}