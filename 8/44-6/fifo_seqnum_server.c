#include <signal.h>
#include "fifo_seqnum.h"

#include <stdio.h>  // for stderr
#include <errno.h>  // for errno
#include <unistd.h>

int main(int argc, char *argv[]) {
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;

    /* Create well-known FIFO, and open it for reading */

    umask(0);   // get permission
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1) {
        perror("open serverFd");
        return 1;
    }

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1) {
        perror("open dummyFd");
        return 1;
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    for (;;) {                          /* Read requests and send responses */
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        // clientFd = open(clientFifo, O_WRONLY);
        for (int i = 0; i < 20000; ++i) {
            if ((clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK)) == -1 && errno == ENXIO)
                continue;
            break;
        }
        if (clientFd == -1) {           /* Open failed, give up on client */
            printf("open %s\n", clientFifo);
            continue;
        }

        /* Send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            printf("close\n");

        seqNum += req.seqLen;           /* Update our sequence number */
    }
}
