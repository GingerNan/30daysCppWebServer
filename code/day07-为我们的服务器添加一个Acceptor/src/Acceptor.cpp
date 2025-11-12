#include "Acceptor.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
    
Acceptor::Acceptor(EventLoop* loop)
    : m_loop(loop)
{
    m_sock = new Socket();
    m_addr = new InetAddress("127.0.0.1", 8888);
    m_sock->bind(m_addr);
    m_sock->listen();
    m_sock->setnonblocking();

    m_acceptChannel = new Channel(m_loop, m_sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    m_acceptChannel->setCallback(cb);
    m_acceptChannel->enbleReading();
}

Acceptor::~Acceptor()
{
    delete m_sock;
    delete m_addr;
    delete m_acceptChannel;
}

void Acceptor::acceptConnection()
{
    m_newConnectionCallback(m_sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb)
{
    m_newConnectionCallback = cb;
}