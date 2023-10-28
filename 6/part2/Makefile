CC = gcc
CFLAGS = -Wall

all: popen_and_pclose

popen_and_pclose: popen_and_pclose.c
	$(CC) $(CFLAGS) -o popen_and_pclose popen_and_pclose.c
	echo "usage: ./popen_and_pclose"

clean:
	rm -f popen_and_pclose
