CC = gcc
CFLAGS = -Wall

all: hostinfo mydate printdir

hostinfo: hostinfo.c
	$(CC) $(CFLAGS) -o hostinfo hostinfo.c

mydate: mydate.c
	$(CC) $(CFLAGS) -o mydate mydate.c

printdir: printdir.c
	$(CC) $(CFLAGS) -o printdir printdir.c

clean:
	rm -f hostinfo mydate printdir
