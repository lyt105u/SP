CC = gcc
CFLAGS = -Wall

# all: server client

# server: server.c
# 	$(CC) $(CFLAGS) -o server server.c

# client: client.c
# 	$(CC) $(CFLAGS) -o client client.c

# clean:
# 	rm -f server client

all: svmsg_file_server svmsg_file_client

svmsg_file_server: svmsg_file_server.c
	$(CC) $(CFLAGS) -o svmsg_file_server svmsg_file_server.c

svmsg_file_client: svmsg_file_client.c
	$(CC) $(CFLAGS) -o svmsg_file_client svmsg_file_client.c

clean:
	rm -f svmsg_file_server svmsg_file_client
