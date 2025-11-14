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
    EventLoop* mainRector_;
    std::vector<EventLoop*> subReactors_;

    Acceptor* acceptor_;
    
    std::map<int, Connection*> connections_;

    ThreadPool* threadPool_;
};