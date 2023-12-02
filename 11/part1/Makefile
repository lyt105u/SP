CC = gcc
CFLAGS = -Wall

all: usock_server usock_client

usock_server: usock_server.c lookup5.c
	$(CC) $(CFLAGS) -o usock_server usock_server.c lookup5.c

usock_client: main.c lookup7.c
	$(CC) $(CFLAGS) -o usock_client main.c lookup7.c

clean:
	rm -f usock_server usock_client
