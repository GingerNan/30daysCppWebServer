#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

#include <cstring>
#include <unistd.h>
#include <iostream>

#define READ_BUFFER 1024

Connection::Connection(EventLoop* loop, Socket* sock)
    : m_loop(loop),
    m_sock(sock),
    m_channel(nullptr)
{
    m_channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    m_channel->setCallback(cb);
    m_channel->enbleReading();
}

Connection::~Connection()
{
    delete m_channel;
    delete m_sock;
}

void Connection::echo(int sockfd)
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

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> cb)
{
    m_deleteConnectionCallback = cb;
}