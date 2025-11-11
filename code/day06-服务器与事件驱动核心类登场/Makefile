server:
	g++ src/util.cpp client.cpp -o client && \
	g++ src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp server.cpp -o server
clean:
	rm -rf server client