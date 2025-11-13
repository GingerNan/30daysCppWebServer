#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"

#include <cstring>
#include <unistd.h>
#include <iostream>

#define READ_BUFFER 1024

Connection::Connection(EventLoop* loop, Socket* sock)
    : m_loop(loop),
    m_sock(sock),
    m_channel(nullptr),
    m_inBuffer(new std::string()),
    m_readBuffer(nullptr)
{
    m_channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    m_channel->setCallback(cb);
    m_channel->enbleReading();
    m_readBuffer = new Buffer();
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
            m_readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, m_readBuffer->c_str());
            int ret = write(sockfd, m_readBuffer->c_str(), m_readBuffer->size());
            errif(ret == -1, "socket write error");
            m_readBuffer->clear();
            break;
        }
        else if (bytes_read == 0)
        {
            //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            //close(sockfd);  //关闭socket会自动将文件描述符从epoll树上移除
            m_deleteConnectionCallback(m_sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> cb)
{
    m_deleteConnectionCallback = cb;
}