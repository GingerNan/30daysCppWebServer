server:
	g++ client.cpp src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp -o client && \
	g++ server.cpp \
	src/util.cpp src/Buffer.cpp src/Connection.cpp src/Acceptor.cpp \
	src/EventLoop.cpp src/Server.cpp src/Epoll.cpp src/InetAddress.cpp \
	src/Socket.cpp src/Channel.cpp src/ThreadPool.cpp \
	-pthread \
	-g -o server
clean:
	rm -rf server client threadPoolTest

threadTest:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o threadPoolTest