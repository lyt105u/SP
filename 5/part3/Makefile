CC = gcc
CFLAGS = -Wall

all: convert file_lookup

convert: convert.c
	$(CC) $(CFLAGS) -o convert convert.c

file_lookup: main.c lookup1.c
	$(CC) $(CFLAGS) -o file_lookup main.c lookup1.c

clean:
	rm -f convert file_lookup
