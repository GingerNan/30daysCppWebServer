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
    EventLoop* m_loop;
    Socket* m_sock;
    //InetAddress* m_addr;
    Channel* m_acceptChannel;
    std::function<void(Socket*)> m_newConnectionCallback;
};