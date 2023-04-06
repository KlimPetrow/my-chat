chat: chat.cpp server.cpp client.cpp
	g++ chat.cpp server.cpp client.cpp -o chat -Wall -Wextra -std=c++11

clean:
	rm chat

.PHONY: clean
