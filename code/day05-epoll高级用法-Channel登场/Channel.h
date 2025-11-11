#pragma once
#include "sys/epoll.h"

class Epoll;
class Channel
{
public:
    Channel(Epoll* ep, int fd);
    ~Channel();

    void enbleReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();

    bool getInEpoll();
    void setInEpoll();

    void setRevents();
private:
    Epoll*  m_ep;
    int m_fd;
    uint32_t m_events;
    uint32_t m_revents;
    bool m_inEpoll;
};