CC = gcc
CFLAGS = -Wall

all: fifo_seqnum_server fifo_seqnum_client fifo_seqnum_client_bad

fifo_seqnum_server: fifo_seqnum_server.c
	$(CC) $(CFLAGS) -o fifo_seqnum_server fifo_seqnum_server.c

fifo_seqnum_client: fifo_seqnum_client.c
	$(CC) $(CFLAGS) -o fifo_seqnum_client fifo_seqnum_client.c

fifo_seqnum_client_bad: fifo_seqnum_client_bad.c
	$(CC) $(CFLAGS) -o fifo_seqnum_client_bad fifo_seqnum_client_bad.c

clean:
	rm -f fifo_seqnum_server fifo_seqnum_client fifo_seqnum_client_bad
