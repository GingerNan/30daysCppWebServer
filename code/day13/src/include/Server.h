#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server
{
public:
    Server(EventLoop* loop);
    ~Server();

    // 新建连接事件
    void newConnection(Socket* sock);

    // 删除连接事件
    void deleteConnection(int sockfd);
private:
    EventLoop* m_mainRector;
    std::vector<EventLoop*> m_subReactors;

    Acceptor* m_acceptor;
    
    std::map<int, Connection*> m_connections;

    ThreadPool* m_threadPool;
};