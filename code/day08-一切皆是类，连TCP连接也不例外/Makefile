server:
	g++ src/util.cpp client.cpp -o client && \
	g++ src/util.cpp src/Connection.cpp src/Acceptor.cpp src/EventLoop.cpp src/Server.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp server.cpp -g -o server
clean:
	rm -rf server client