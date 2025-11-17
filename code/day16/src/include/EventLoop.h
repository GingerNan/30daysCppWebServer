#pragma once
#include "common.h"
#include <memory>

class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
public:
    EventLoop();
    ~EventLoop();

    void Loop() const;
    
    void UpdateChannel(Channel* ch);
    void DeleteChannel(Channel* ch);
private:
    std::unique_ptr<Poller> poller_;
};