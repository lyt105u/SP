#include "svmsg_file.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mqueue.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

// SIGCHLD handler
static void grimReaper(int sig) {
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

// Executed in child process: serve a single client
static void serveRequest(const struct requestMsg *req) {
    int fd;
    ssize_t numRead;
    struct responseMsg resp;

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {                     /* Open failed: send error text */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        // msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        int status = mq_send(req->clientId, &resp, strlen(&resp), 0);
        if (status == -1) {
            perror("mq_send");
            exit(1);
        }
        exit(EXIT_FAILURE);             /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA. We don't
       diagnose read() and msgsnd() errors since we can't notify client. */

    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0)
        // if (msgsnd(req->clientId, &resp, numRead, 0) == -1)
        if(mq_send(req->clientId, &resp, numRead, 0))
            break;

    /* Send a message of type RESP_MT_END to signify end-of-file */

    resp.mtype = RESP_MT_END;
    // msgsnd(req->clientId, &resp, 0, 0);         /* Zero-length mtext */
    mq_send(req->clientId, &resp, 0, 0);
}

void errExit(char *err) {
    perror(err);
    return;
}

void errMsg(char *err) {
    printf("%s\n", err);
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    int serverId;
    struct sigaction sa;

    struct mq_attr attr;

    /* Create server message queue */

    // serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL |
    //                         S_IRUSR | S_IWUSR | S_IWGRP);
    // if (serverId == -1)
    //     errExit("msgget");
    serverId = mq_open(SERVER_KEY, O_CREAT | O_RDWR, 0644, NULL);
    if (serverId == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    /* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Read requests, handle each in a separate child process */

    for (;;) {
        // msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        mq_getattr(serverId, &req);
        char buffer[1024];
        ssize_t bytes_read = mq_receive(req, buffer, attr.mq_msgsize, NULL);

        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            errMsg("msgrcv");           /* Some other error */
            break;                      /* ... so terminate loop */
        }

        pid = fork();                   /* Create child process */
        if (pid == -1) {
            errMsg("fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If msgrcv() or fork() fails, remove server MQ and exit */

    if (msgctl(serverId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
    exit(EXIT_SUCCESS);
}
