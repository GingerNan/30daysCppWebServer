#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>

#define MAX_EVENTS 1000

Epoll::Epoll() :epfd_(-1), events_(nullptr)
{
    epfd_ = epoll_create1(0);
    ErrorIf(epfd_ == -1, "epoll create error");
    events_ = new epoll_event[MAX_EVENTS];
    bzero(events_, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if (epfd_ != -1)
    {
        close(epfd_);
        epfd_ = -1;
    }
    delete[] events_;
}

void Epoll::AddFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.data.fd = fd;
    ev.events = op;
    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    ErrorIf(ret == -1, "erpoll add event error");
}

std::vector<Channel*> Epoll::Poll(int timeout)
{
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    ErrorIf(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i)
    {
        Channel* ch = (Channel*)events_[i].data.ptr;
        ch->SetReadyEvents(events_[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoll::UpdateChannel(Channel* ch)
{
    int fd = ch->GetFd();
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.data.ptr = ch;
    ev.events = ch->GetListenEvents();
    if (!ch->GetInEpoll())
    {
        int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
        ErrorIf(ret == -1, "epoll add error");
        ch->SetInEpoll();
    }
    else
    {
        int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
        ErrorIf(ret == -1, "epoll modify error");
    }
}

void Epoll::DeleteChannel(Channel* ch)
{
    int fd = ch->GetFd();
    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
    ErrorIf(ret == -1, "epoll delete error");
    ch->SetInEpoll(false);
}

//std::vector<epoll_event> Epoll::poll(int timeout)
//{
//    std::vector<epoll_event> active_events;
//    //TODO epoll wait error: Bad address
//    int nfds = epoll_wait(m_epfd, m_events, MAX_EVENTS, timeout);
//    ErrorIf(nfds == -1, "epoll wait error");
//    for (int i = 0; i < nfds; ++i)
//    {
//        active_events.push_back(m_events[i]);
//    }
//    return active_events;
//}