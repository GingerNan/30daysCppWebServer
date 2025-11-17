#pragma once
#include "common.h"
#include <functional>
#include <memory>

class Acceptor
{
public:
    DISALLOW_COPY_AND_MOVE(Acceptor);
public:
    explicit Acceptor(EventLoop* loop);
    ~Acceptor();

    RC AcceptConnection() const;

    void SetNewConnectionCallback(std::function<void(int)> const& callback);
private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channe_;
    std::function<void(int)> new_connection_callback_;
};