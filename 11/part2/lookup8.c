/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "dict.h"

int lookup(char * buffer, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;
	int n;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			perror("ERROR opening socket");
			return 1;
		}

		host = gethostbyname(resource);
		if (host == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}
		// server.sin_family = AF_INET;
		/* Fill in code. */

		/* Allocate socket.
		 * Fill in code. */
		bzero((char *) &server, sizeof(server));
		server.sin_family = AF_INET;
		bcopy((char *)host->h_addr_list[0], (char *)&server.sin_addr.s_addr, host->h_length);
		server.sin_port = htons(PORT);

		/* Connect to the server.
		 * Fill in code. */
		if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        	perror("ERROR connecting");
			return 1;
		}
	}

	/* write query on socket ; await reply
	 * Fill in code. */
    n = write(sockfd, buffer, strlen(buffer)+1);
    if (n < 0) {
        perror("ERROR writing to socket");
		return 1;
	}

	// bzero(buffer, TEXT);
    n = read(sockfd, buffer, TEXT);
    if (n < 0) {
        perror("ERROR reading from socket");
		return 1;
	}

	if (strcmp(buffer,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
