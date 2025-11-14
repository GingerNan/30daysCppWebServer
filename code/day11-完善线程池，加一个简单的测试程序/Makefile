src=$(wildcard src/*.cpp)

server:
	g++ -std=c++11 -pthread -g \
	$(src) \
	server.cpp \
 	-o server
client:
	g++ client.cpp src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp -o client

th:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o threadPoolTest

test:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp src/ThreadPool.cpp \
	-pthread -g \
	test.cpp -o test

clean:
	rm server && rm client && rm test