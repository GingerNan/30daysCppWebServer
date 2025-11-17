#pragma once
#include "common.h"
#include <functional>

class Channel
{
public:
    DISALLOW_COPY_AND_MOVE(Channel);
public:
    Channel(int fd, EventLoop* loop);
    ~Channel();

    void HandleEvent() const;

    void EnbleWrite();
    void EnbleRead();

    int GetFd() const;

    short GetListenEvents() const;
    short GetReadyEvents() const;

    bool GetExist();
    void SetExist(bool in = true);

    void EnbleET();

    void SetReadyEvents(short ev);
    void SetReadCallback(std::function<void()> const& callback);
    void SetWriteCallback(std::function<void()> const& callback);

    static const int READ_EVENT;
    static const int WRITE_EVENT;
    static const int ET;
private:
    int fd_;
    EventLoop* loop_;

    short listen_events_;
    short ready_events_;

    bool exist_;

    std::function<void()> read_callback_;
    std::function<void()> write_callback_;
};