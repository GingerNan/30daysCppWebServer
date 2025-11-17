#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"

#include <unistd.h>
#include <cstring>

Connection::Connection(int fd, EventLoop* loop)
{
    socket_ = std::make_unique<Socket>();
    socket_->SetFd(fd);
    
    if (loop != nullptr)
    {
        channel_ = std::make_unique<Channel>(fd, loop);
        channel_->EnbleRead();
        channel_->EnbleET();
    }

    read_buf_ = std::make_unique<Buffer>();
    send_buf_ = std::make_unique<Buffer>();

    state_ = State::Connected;
}

Connection::~Connection()
{
}

RC Connection::Read()
{
    if (state_ != State::Connected)
    {
        perror("Connection is not connected, can not read");
        return RC_CONNECTION_ERROR;
    }
    assert(state_ == State::Connected && "connection state is disconnected!");
    read_buf_->Clear();
    if (socket_->IsNonBlocking())
    {
        return ReadNonBlocking();
    }
    else
    {
        return ReadBlocking();
    }
}

RC Connection::Write()
{
    if (state_ != State::Connected)
    {
        perror("Connection is not connected, can not read");
        return RC_CONNECTION_ERROR;
    }

    RC rc = RC_UNDEFINED;
    if (socket_->IsNonBlocking())
    {
        rc = WriteNonBlocking();
    }
    else
    {
        rc = WriteBlocking();
    }
    send_buf_->Clear();
    return rc;
}


RC Connection::Send(std::string msg)
{
    SetSendBuffer(msg.c_str());
    Write();
    return RC_SUCCESS;
}


RC Connection::ReadNonBlocking()
{
    int sockfd = socket_->GetFd();
    char buf[1024];
    while (true)    // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    {
        memset(buf, 0, sizeof buf);
        ssize_t bytes_read = ::read(sockfd, buf, sizeof buf);
        if (bytes_read > 0)
        {
            read_buf_->Append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR) //程序正常中断，继续读取
        {
            printf("continue reading\n");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 非阻塞IO，这个条件表示数据全部读取完毕
        {
            break;
        }
        else if (bytes_read == 0) // EOF，客户端断开连接
        {
            printf("read EOF, client fd %d disconnected\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }
        else{
            printf("Other error on clinet fd %d\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }
    }
    return RC_SUCCESS;
}

RC Connection::WriteNonBlocking()
{
    int sockfd = socket_->GetFd();
    char buf[send_buf_->Size()];
    memcpy(buf, send_buf_->c_str(), send_buf_->Size());
    int data_size = send_buf_->Size();
    int data_left = data_size;
    while (data_left > 0)
    {
        ssize_t bytes_write = ::write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR)
        {
            printf("continue writing\n");
            continue;
        }

        if (bytes_write == -1 && errno == EAGAIN)
        {
            break;
        }

        if (bytes_write == -1)
        {
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
    return RC_SUCCESS;
}

RC Connection::ReadBlocking()
{
    int sockfd = socket_->GetFd();
    // unsigned int rcv_size = 0;
    // socklen_t len = sizeof rcv_size;
    // getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);

    size_t data_size = socket_->RecvBufSize();
    char buf[1024];
    ssize_t bytes_read = ::read(sockfd, buf, sizeof buf);
    if (bytes_read > 0)
    {
        read_buf_->Append(buf, bytes_read);
    }
    else if (bytes_read == 0)
    {
        printf("read EOF, blocking client fd %d disconnected\n", sockfd);
        state_ = State::Closed;
    }
    else if (bytes_read == -1)
    {
        printf("[ReadBlocking] Other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
    return RC_SUCCESS;
}

RC Connection::WriteBlocking()
{
    // 没有处理send_buffer_数据大于TCP写缓冲区的情况，可能会有bug
    int sockfd = socket_->GetFd();
    ssize_t bytes_write = ::write(sockfd, send_buf_->c_str(), send_buf_->Size());
    if (bytes_write == -1)
    {
        printf("[WriteBlocking]Other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
    return RC_SUCCESS;
}

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const& callback)
{
    delete_connection_ = callback;
}

void Connection::SetOnRecv(std::function<void(Connection*)> const& callback)
{
    on_recv_ = std::move(callback);
    std::function<void()> bus = std::bind(&Connection::Business, this);
    channel_->SetReadCallback(bus);
}

void Connection::Close()
{
    delete_connection_(socket_->GetFd());
}

Connection::State Connection::GetState() const
{
    return state_;
}

Socket* Connection::GetSocket() const
{
    return socket_.get();
}

void Connection::SetSendBuffer(const char* str)
{
    send_buf_->SetBuf(str);
}

Buffer* Connection::GetReadBuffer()
{
    return read_buf_.get();
}

Buffer* Connection::GetSendBuffer()
{
    return send_buf_.get();
}
