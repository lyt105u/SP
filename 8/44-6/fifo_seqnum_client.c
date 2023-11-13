#include "fifo_seqnum.h"

#include <errno.h>  // for errno
#include <stdlib.h> // for EXIT_SUCCESS
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static char clientFifo[CLIENT_FIFO_NAME_LEN];

/* Invoked on exit to delete client FIFO */
static void removeFifo(void) {
    unlink(clientFifo);
}

int main(int argc, char *argv[]) {
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s [seq-len]\n", argv[0]);
        return 1;
    }

    /* Create our FIFO (before sending request, to avoid a race) */

    umask(0);                   /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        printf("mkfifo %s\n", clientFifo);
        return 1;
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit");
        return 1;
    }

    /* Construct request message, open server FIFO, and send message */

    req.pid = getpid();
    req.seqLen = atoi(argv[1]);

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        printf("open %s\n", SERVER_FIFO);
        return 1;
    }

    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
        printf("Can't write to server\n");
        return 1;
    }

    /* Open our FIFO, read and display response */

    clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        printf("open %s\n", clientFifo);
        return 1;
    }

    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
        printf("Can't read response from server\n");
        return 1;
    }

    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
