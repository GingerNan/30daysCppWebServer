#pragma once

class EventLoop;
class Socket;
class Server
{
public:
    Server(EventLoop* loop);
    ~Server();

    // 读事件
    void handleReadEvent(int);

    // 新建连接事件
    void newConnection(Socket* serv_sock);
private:
    EventLoop* m_loop;
};