/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include "dict.h"

#define BUFFER_SIZE 480
#define WORD_SIZE 32

int main(int argc, char **argv) {
    struct sockaddr_un server;
    int sd, cd;
	socklen_t address_length;
	char buffer[BUFFER_SIZE];
	char word[WORD_SIZE];

    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source>"
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    // Setup socket.
	sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize address.
	memset(&server, 0, sizeof(struct sockaddr_un));
    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, argv[2], sizeof(server.sun_path) - 1);

    // Name and activate the socket.
	unlink(argv[2]);
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) != 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sd, 5) != 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    /* main loop : accept connection; fork a child to have dialogue */
    for (;;) {
		// Wait for a connection.
		cd = accept(sd, (struct sockaddr *)&server, &address_length);
        if (cd < 0) {
            perror("accept failed");
            continue;  // Continue to the next iteration of the loop
        }

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				// Get next request
				while (1) {
					memset(buffer, 0, BUFFER_SIZE);
					ssize_t read_bytes = read(cd, buffer, BUFFER_SIZE - 1);

					if (read_bytes <= 0) {
						// Either an error occurred or the client closed the connection
						break;
					}

					memcpy(word, buffer, sizeof(word));
					// write(cd, buffer, read_bytes);

					/* Lookup request. */
					ssize_t written;
					switch(lookup(buffer, argv[1])) {
						/* Write response back to client. */
						case FOUND: 
							/* Fill in code. */
							written = write(cd, buffer, sizeof(buffer));
							if (written != sizeof(buffer)) {
								// Handle partial write or error
								perror("write");
							}
							break;
						case NOTFOUND: 
							/* Fill in code. */
							written = write(cd, buffer, sizeof(buffer));
							if (written != sizeof(buffer)) {
								// Handle partial write or error
								perror("write");
							}
							break;
						case UNAVAIL:
							DIE(argv[1]);
					}

				}

				/* Terminate child process.  It is done. */
				exit(0);

			default :
				close(cd);
				break;
		}
    }
}
