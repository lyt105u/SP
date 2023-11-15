CC = gcc
CFLAGS = -Wall

all: svmsg_file_server svmsg_file_client

svmsg_file_server: svmsg_file_server.c
	$(CC) $(CFLAGS) -o svmsg_file_server svmsg_file_server.c

svmsg_file_client: svmsg_file_client.c
	$(CC) $(CFLAGS) -o svmsg_file_client svmsg_file_client.c

clean:
	rm -f svmsg_file_server svmsg_file_client
