#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
public:
    Acceptor(EventLoop* loop);
    ~Acceptor();

    void acceptConnection();

    void setNewConnectionCallback(std::function<void(Socket*)>);
private:
    EventLoop* loop_;
    Socket* sock_;
    //InetAddress* m_addr;
    Channel* acceptChannel_;
    std::function<void(Socket*)> newConnectionCallback_;
};