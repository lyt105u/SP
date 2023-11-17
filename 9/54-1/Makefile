CC = gcc
CFLAGS = -Wall

all: svshm_xfr_reader svshm_xfr_writer

svshm_xfr_reader: svshm_xfr_reader.c
	$(CC) $(CFLAGS) -o svshm_xfr_reader svshm_xfr_reader.c -lpthread

svshm_xfr_writer: svshm_xfr_writer.c
	$(CC) $(CFLAGS) -o svshm_xfr_writer svshm_xfr_writer.c -lpthread

clean:
	rm -f svshm_xfr_reader svshm_xfr_writer
