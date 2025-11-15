#pragma once
#include "Macros.h"
#include <functional>

class EventLoop;
class Socket;
class Channel;

class Acceptor
{
public:
    Acceptor(EventLoop* loop);
    ~Acceptor();

    DISALLOW_COPY_AND_MOVE(Acceptor);

    void acceptConnection();

    void setNewConnectionCallback(std::function<void(Socket*)>);
private:
    EventLoop* loop_;
    Socket* sock_;
    Channel* acceptChannel_;
    std::function<void(Socket*)> newConnectionCallback_;
};