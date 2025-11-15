#include <iostream>
#include <unistd.h>
#include <cstring>
#include "util.h"
#include "Buffer.h"
#include "Socket.h"

int main()
{
    Socket* sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 8888);
    sock->Connect(addr);

    int sockfd = sock->GetFd();

    Buffer* sendBuffer = new Buffer();
    Buffer* readBuffer = new Buffer();

    while (true)
    {
        sendBuffer->Getline();
        ssize_t write_bytes = write(sockfd, sendBuffer->ToStr(), sendBuffer->Size());
        if (write_bytes == -1)
        {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        int already_read = 0;
        char buf[1024];
        while (true)
        {
            bzero(&buf, sizeof buf);
            ssize_t read_bytes = read(sockfd, buf, sizeof buf);

            if (read_bytes > 0)
            {
                readBuffer->Append(buf, read_bytes);
                already_read += read_bytes;
            }
            else if (read_bytes == 0)  // EOF
            {
                printf("server socket disconnected!\n");
                exit(EXIT_SUCCESS);
            }

            if (already_read >= sendBuffer->Size())
            {
                printf("message from server: %s\n", readBuffer->ToStr());
                break;
            }
        }
        readBuffer->Clear();
    }

    delete addr;
    delete sock;
    
    return 0;
}