#include "src/util.h"
#include "src/Epoll.h"
#include "src/Socket.h"
#include "src/InetAddress.h"
#include "src/Channel.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>


#define READ_BUFFER 1024

void handleReadEvent(int);

int main()
{
    Socket* serv_sock = new Socket();
    InetAddress* serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    Epoll* ep = new Epoll();
    serv_sock->setnonblocking();

    Channel* serverChannel = new Channel(ep, serv_sock->getFd());
    serverChannel->enbleReading();

    while (true)
    {
        std::vector<Channel*> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        for (int i = 0; i < nfds; i++)
        {
            int chfd = activeChannels[i]->getFd();
            if (chfd == serv_sock->getFd())
            {
                // 新客户端连接
                InetAddress* client_addr = new InetAddress();                       // 会发生内存泄露！没有delete
                Socket* client_sock = new Socket(serv_sock->accept(client_addr));   // 会发生内存泄露！没有delete
                printf("new client fd=%d! %s Port:%d\n", 
                    client_sock->getFd(),
                    inet_ntoa(client_addr->m_addr.sin_addr),
                    ntohs(client_addr->m_addr.sin_port)
                );

                client_sock->setnonblocking();
                Channel* clientChannel = new Channel(ep, client_sock->getFd());
                clientChannel->enbleReading();
            }
            else if (activeChannels[i]->getRevents() & EPOLLIN)
            {
                //可读事件
                handleReadEvent(activeChannels[i]->getFd());
            }
            else
            {
                printf("something else happened\n");
            }
        }
    }
    
    delete serv_addr;
    delete serv_sock;

    return 0;
}

void handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    while (true)
    {
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof buf);
        if (bytes_read > 0)
        {
            printf("message from client fd %d:%s\n", sockfd, buf);
            write(sockfd, buf, sizeof buf);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN || (errno == EWOULDBLOCK)))) //非阻塞IO，这个条件表示数据全部读取完毕
        {
            printf("finish reading once, errno:%d\n", errno);
            break;
        }
        else if (bytes_read == 0)   // EOF，客户端断开连接
        {
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);  //关闭socket会自动将文件描述符从epoll数上移除
            break;
        }
    }
    
}