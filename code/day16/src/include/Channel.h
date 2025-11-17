#pragma once
#include "Macros.h"

#include <functional>
#include <cstdint>

class Socket;
class EventLoop;

class Channel
{
public:
    Channel(EventLoop* loop, Socket* socket);
    ~Channel();

    DISALLOW_COPY_AND_MOVE(Channel);

    void HandleEvent();

    void EnbleWrite();
    void EnbleRead();

    Socket* GetSocket();

    uint32_t GetListenEvents();
    uint32_t GetReadyEvents();

    bool GetExist();
    void SetExist(bool in = true);

    void UseET();

    void SetReadyEvents(uint32_t);
    void SetReadCallback(std::function<void()> const& callback);
    void SetWriteCallback(std::function<void()> const& callback);

    static const int READ_EVENT;
    static const int WRITE_EVENT;
    static const int ET;
private:
    EventLoop* loop_;
    Socket* socket_;

    uint32_t listen_events_{0};
    uint32_t ready_events_{0};

    bool exist_{false};

    std::function<void()> read_callback_;
    std::function<void()> write_callback_;
};