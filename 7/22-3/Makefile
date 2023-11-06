CC = gcc
CFLAGS = -Wall

all: sig_speed_sigsuspend sig_speed_sigwaitinfo

sig_speed_sigsuspend: sig_speed_sigsuspend.c
	$(CC) $(CFLAGS) -o sig_speed_sigsuspend sig_speed_sigsuspend.c

sig_speed_sigwaitinfo: sig_speed_sigwaitinfo.c
	$(CC) $(CFLAGS) -o sig_speed_sigwaitinfo sig_speed_sigwaitinfo.c

clean:
	rm -f sig_speed_sigsuspend sig_speed_sigwaitinfo
