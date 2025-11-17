#pragma once
#include "common.h"
#include <functional>
#include <memory>

class Connection
{
public:
    DISALLOW_COPY_AND_MOVE(Connection);

    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
public:
    Connection(int fd, EventLoop* loop);
    ~Connection();

    void SetDeleteConnectionCallback(std::function<void(int)> const& callback);
    void SetOnConnect(std::function<void(Connection*)> const& callback);
    void SetOnRecv(std::function<void(Connection*)> const& callback);

    State GetState() const;
    Socket* GetSocket() const;

    Buffer* GetReadBuffer();
    void SetSendBuffer(const char* str);
    Buffer* GetSendBuffer();

    RC Read();
    RC Write();
    RC Send(std::string msg);

    void Close();

    void OnConnect(std::function<void()> fn);
    void OnMessage(std::function<void()> fn);
private:
    void Business();

    RC ReadNonBlocking();
    RC WriteNonBlocking();
    RC ReadBlocking();
    RC WriteBlocking();
private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    State state_;
    std::unique_ptr<Buffer> read_buf_;
    std::unique_ptr<Buffer> send_buf_;

    std::function<void(int)> delete_connection_;
    std::function<void(Connection*)> on_recv_;
};