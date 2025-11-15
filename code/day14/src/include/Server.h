#pragma once
#include "Macros.h"

#include <functional>
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

    DISALLOW_COPY_AND_MOVE(Server)

    // 新建连接事件
    void NewConnection(Socket* sock);

    // 删除连接事件
    void DeleteConnection(Socket* sock);

    void OnConnect(std::function<void(Connection*)> fn);
private:
    EventLoop* mainRector_;
    std::vector<EventLoop*> subReactors_;

    Acceptor* acceptor_;
    
    std::map<int, Connection*> connections_;

    ThreadPool* threadPool_;

    std::function<void(Connection*)> on_connect_callback_;
};