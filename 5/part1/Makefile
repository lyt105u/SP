CC = gcc
CFLAGS = -Wall

all: hostinfo mydate printdir mycat pipe_ls

hostinfo: hostinfo.c
	$(CC) $(CFLAGS) -o hostinfo hostinfo.c

mydate: mydate.c
	$(CC) $(CFLAGS) -o mydate mydate.c

printdir: printdir.c
	$(CC) $(CFLAGS) -o printdir printdir.c

mycat: mycat.c
	$(CC) $(CFLAGS) -o mycat mycat.c

pipe_ls: pipe_ls.c
	$(CC) $(CFLAGS) -o pipe_ls pipe_ls.c

clean:
	rm -f hostinfo mydate printdir mycat pipe_ls
