/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "dict.h"

int lookup(char * buffer, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	static int first_time = 1;
	char server_ip[16];

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		// Set up destination address.
		if( strcmp(resource, "localhost") == 0 ) {
			strcpy(server_ip, "127.0.0.1");
		} else {
			strcpy(server_ip, resource);
		}

		// Creating socket file descriptor
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}

		// Allocate a socket.
		memset(&server, 0, sizeof(server));

		// Filling server information
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr(server_ip);
	}

	// Send a datagram & await reply
	sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &server, sizeof(server));

	// Receiving response from server
    int n;
    socklen_t len = sizeof(server);
    n = recvfrom(sockfd, (char *)buffer, TEXT, MSG_WAITALL, (struct sockaddr *) &server, &len);
    buffer[n] = '\0';

	if (strcmp(buffer,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
