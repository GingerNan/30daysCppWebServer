#include "Server.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop)
    : m_loop(loop)
{
    Socket* serv_sock = new Socket();
    InetAddress* serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    serv_sock->setnonblocking();

    Channel* serv_channel = new Channel(m_loop, serv_sock->getFd());
    std::function<void()> cb = std::bind(&Server::newConnection, this, serv_sock);
    serv_channel->setCallback(cb);
    serv_channel->enbleReading();
}

Server::~Server()
{

}

void Server::handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    {
        // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof buf);
        ssize_t bytes_read = read(sockfd, buf, sizeof buf);
        if (bytes_read > 0)
        {
            printf("message from client fd:%d %s", sockfd, buf);
            write(sockfd, buf, sizeof buf);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if (bytes_read == 0)
        {
            //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);  //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
    
}

void Server::newConnection(Socket* serv_sock)
{
    InetAddress* client_addr = new InetAddress();                        // 会发生内存泄露！没有delete
    Socket* client_sock = new Socket(serv_sock->accept(client_addr));    // 会发生内存泄漏！没有delete
    printf("new client fd:%d! IP:%s Port:%d\n", 
        client_sock->getFd(),
        inet_ntoa(client_addr->m_addr.sin_addr),
        ntohs(client_addr->m_addr.sin_port)
    );

    client_sock->setnonblocking();
    Channel* client_channel = new Channel(m_loop, client_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, client_sock->getFd());
    client_channel->setCallback(cb);
    client_channel->enbleReading();
}