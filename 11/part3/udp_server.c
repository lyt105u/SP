/*
 * udp_server : listen on a UDP socket ;reply immediately
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server, client;
	int sockfd, n;
	socklen_t siz;
	char buffer[TEXT];

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}

	// Create a UDP socket.
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

	// Initialize address.
	memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
	
	server.sin_family = AF_INET; // IPv4
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

	// Name and activate the socket.
	if (bind(sockfd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

	for (;;) { /* await client packet; respond immediately */

		siz = sizeof(client); /* siz must be non-zero */

		// Wait for a request.
		n = recvfrom(sockfd, (char *)buffer, TEXT, MSG_WAITALL, (struct sockaddr *) &client, &siz);
		buffer[n] = '\0';

		switch(lookup(buffer, argv[1]) ) {
			case FOUND:
				sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client, siz);
				break;
			case NOTFOUND:
				sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &client, siz);
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
	}
}
