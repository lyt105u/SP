/*
 * lookup7 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Unix TCP Sockets
 * The name of the socket is passed as resource.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "dict.h"

int lookup(char * buffer, const char * resource) {
	static int sockfd;
	static struct sockaddr_un server;
	static int first_time = 1;

	if (first_time) {     /* connect to socket ; resource is socket name */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
		sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
		if (sockfd < 0) {
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		/* Allocate socket. */
		memset(&server, 0, sizeof(struct sockaddr_un));
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path,resource);

		/* Connect to the server.
		 * Fill in code. */
		if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) != 0) {
			perror("connect failed");
			exit(EXIT_FAILURE);
		}
	}

	/* write query on socket ; await reply
	 * Fill in code. */
	write(sockfd, buffer, strlen(buffer) + 1);

	read(sockfd, buffer, 480);

	if (strcmp(buffer,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
