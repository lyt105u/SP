CC = gcc
CFLAGS = -Wall

all: pipe_communication

pipe_communication: pipe_communication.c
	$(CC) $(CFLAGS) -o pipe_communication pipe_communication.c
	echo "usage: ./pipe_communication"

clean:
	rm -f pipe_communication
