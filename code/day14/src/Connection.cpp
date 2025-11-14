#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <cassert>
#include <utility>


Connection::Connection(EventLoop* loop, Socket* sock)
    : loop_(loop),
    sock_(sock),
{
    if (loop_ != nullptr)
    {
        channel_ = new Channel(loop, sock->getFd());
        channel_->enbleReading();
        channel_->useET();
    }

    read_buffer_ = new Buffer();
    send_buffer_ = new Buffer();
    state_ = State::Connected;
}

Connection::~Connection()
{
    if (loop_ == nullptr)
    {
        delete channel_;
    }
    
    delete sock_;
    delete read_buffer_;
    delete send_buffer_;
}

void Connection::Read()
{
    ASSERT(state_ == State::Connected, "connection state is disconnected!");
    read_buffer_->clear();
}

void Connection::Write()
{

}

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> const& callback)
{

}

void Connection::SetOnConnectCallback(std::function<void(Connection*)> const& callback)
{

}

void Connection::Close()
{

}

Buffer* Connection::GetReadBUffer()
{

}

const char* Connection::ReadBuffer()
{

}

void Connection::SetSendBuffer(const char* str)
{

}

Buffer* Connection::GetSendBuffer()
{

}

const char* Connection::SendBuffer()
{

}

void Connection::GetlineSendBuffer()
{

}

Socket* Connection::GetSocket()
{

}

void Connection::OnConnect(std::function<void()> fn)
{

}

void Connection::ReadNonBlocking()
{

}

void Connection::WriteNonBlocking()
{

}

void Connection::ReadBlocking()
{

}

void Connection::WriteBlocking()
{
    
}