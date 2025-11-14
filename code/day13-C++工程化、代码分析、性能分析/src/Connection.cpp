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
    m_readBuffer(nullptr)
{
    m_channel = new Channel(loop, sock->getFd());
    m_channel->enbleReading();
    m_channel->useET();

    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    m_channel->setReadCallback(cb);
    m_channel->setUseThreadPool(true);
    
    m_readBuffer = new Buffer();
}

Connection::~Connection()
{
    delete m_channel;
    delete m_sock;
    delete m_readBuffer;
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
            m_deleteConnectionCallback(sockfd);     // 多线程会有bug
            break;
        }
        else
        {
            printf("Connection reset by peer\n");
            m_deleteConnectionCallback(sockfd);     // 会有bug，注释后单线程无bug
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> cb)
{
    m_deleteConnectionCallback = cb;
}

void Connection::send(int sockfd)
{
    char buf[m_readBuffer->size()];
    strcpy(buf, m_readBuffer->c_str());
    int data_size = m_readBuffer->size();
    int data_left = data_size;
    while (data_left > 0)
    {
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EAGAIN)
        {
            break;
        }
        data_left -= bytes_write;
    }
}