#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;

class Connection
{
public:
    Connection(EventLoop* loop, Socket* sock);
    ~Connection();

    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
private:
    EventLoop* m_loop;
    Socket* m_sock;
    Channel* m_channel;
    std::function<void(Socket*)> m_deleteConnectionCallback;
};