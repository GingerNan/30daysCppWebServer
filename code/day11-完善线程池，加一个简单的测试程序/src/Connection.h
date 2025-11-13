#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection
{
public:
    Connection(EventLoop* loop, Socket* sock);
    ~Connection();

    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(int)>);
    void send(int sockfd);
private:
    EventLoop* m_loop;
    Socket* m_sock;
    Channel* m_channel;
    std::function<void(int)> m_deleteConnectionCallback;
    std::string* m_inBuffer;
    Buffer* m_readBuffer;
};