#pragma once
#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server
{
public:
    Server(EventLoop* loop);
    ~Server();

    // 新建连接事件
    void newConnection(Socket* serv_sock);

    // 删除连接事件
    void deleteConnection(Socket* sock);
private:
    EventLoop* m_loop;
    Acceptor* m_acceptor;
    std::map<int, Connection*> m_connections;
};