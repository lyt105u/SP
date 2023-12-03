/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	// create two sockets.
	// server: linstens from multiple sources
	// client: specific client-server communication
	static struct sockaddr_in server, client;
	int sd,cd,n;
	socklen_t clilen;
	char buffer[TEXT];
	char word[WORD];

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	// Create the socket.
	sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
		perror("ERROR opening socket");
		return 1;
	}

	// Initialize address.
	bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;	// local host
    server.sin_port = htons(PORT);

	// Name and activate the socket.
	if (bind(sd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("ERROR on binding");
		return 1;
	}

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		// Wait for a connection.
		listen(sd, 5);
		clilen = sizeof(client);
		cd = accept(sd, (struct sockaddr *) &client, &clilen);
		if (cd < 0) {
			perror("ERROR on accept");
			return 1;
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				// Get next request.
				while (1) {
					bzero(buffer, TEXT);
					n = read(cd, buffer, TEXT-1);
					if (n < 0) {
						perror("ERROR reading from socket");
						return 1;
					}

					memcpy(word, buffer, sizeof(word));


					/* Lookup the word , handling the different cases appropriately */
					ssize_t written;
					switch(lookup(buffer, argv[1]) ) {
						/* Write response back to the client. */
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
				exit(0);

			default :
				close(cd);
				break;
		}
	}
}
