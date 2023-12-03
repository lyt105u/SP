CC = gcc
CFLAGS = -Wall

all: isock_server isock_client

isock_server: isock_server.c lookup5.c
	$(CC) $(CFLAGS) -o isock_server isock_server.c lookup5.c

isock_client: main.c lookup8.c
	$(CC) $(CFLAGS) -o isock_client main.c lookup8.c

clean:
	rm -f isock_server isock_client
