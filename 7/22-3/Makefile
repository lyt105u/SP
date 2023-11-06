CC = gcc
CFLAGS = -Wall

all: sig_speed_comparison

sig_speed_comparison: sig_speed_comparison.c
	$(CC) $(CFLAGS) -o sig_speed_comparison sig_speed_comparison.c
	echo "usage: ./sig_speed_comparison num-sigs"

clean:
	rm -f sig_speed_comparison
