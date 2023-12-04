CC = gcc
CFLAGS = -Wall

all: udp_server udp_client

udp_server: udp_server.c lookup5.c
	$(CC) $(CFLAGS) -o udp_server udp_server.c lookup5.c

udp_client: main.c lookup9.c
	$(CC) $(CFLAGS) -o udp_client main.c lookup9.c

clean:
	rm -f udp_server udp_client
