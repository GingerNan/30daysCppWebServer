#pragma once
#include "Macros.h"
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection
{
public:
    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };

    Connection(EventLoop* loop, Socket* sock);
    ~Connection();
    DISALLOW_COPY_AND_MOVE(Connection);

    void Read();
    void Write();

    void SetDeleteConnectionCallback(std::function<void(Socket*)> const& callback);
    void SetOnConnectCallback(std::function<void(Connection*)> const& callback);

    State GetState() { return state_; }
    void Close();

    Buffer* GetReadBUffer();
    const char* ReadBuffer();

    void SetSendBuffer(const char* str);
    Buffer* GetSendBuffer();
    const char* SendBuffer();

    void GetlineSendBuffer();

    Socket* GetSocket();

    void OnConnect(std::function<void()> fn);
private:
    EventLoop* loop_;
    
    Socket* sock_;
    
    Channel* channel_{nullptr};

    State state_{State::Invalid};

    Buffer* read_buffer_;
    Buffer* send_buffer_;

    std::function<void(Socket*)> delete_connection_callback_;
    std::function<void(Connection*)> delete_connection_callback_;

    void ReadNonBlocking();
    void WriteNonBlocking();
    void ReadBlocking();
    void WriteBlocking();
};