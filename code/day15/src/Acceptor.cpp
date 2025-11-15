#include "Acceptor.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

#include <iostream>
    
Acceptor::Acceptor(EventLoop* loop)
    : loop_(loop),
    sock_(nullptr),
    acceptChannel_(nullptr)
{
    sock_ = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 8888);
    sock_->Bind(addr);
    sock_->Listen();
    /*
        对于Acceptor，接受连接的处理事件比较短、报文数据极小，并且一般不会有特别多的新连接在同一
    时间到达，所有Accept没有必要采用epoll ET模式，也没有必要用线程池。由于不会成为性能瓶颈。
    */
    //m_sock->setnonblocking(); acceptor使用阻塞式IO比较好

    acceptChannel_ = new Channel(loop_, sock_);
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel_->SetReadCallback(cb);
    acceptChannel_->EnbleRead();

    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock_;
    delete acceptChannel_;
}

void Acceptor::acceptConnection()
{
    InetAddress* client_addr = new InetAddress();
    Socket* client_sock = new Socket(sock_->Accept(client_addr));
    
    printf("new client fd %d! IP:%s Port:%d\n", client_sock->GetFd()
        , client_addr->GetIp()
        , client_addr->GetPort());

    client_sock->Setnonblocking();  // 新接收到的连接设置为非阻塞模式
    newConnectionCallback_(client_sock);
    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb)
{
    newConnectionCallback_ = cb;
}