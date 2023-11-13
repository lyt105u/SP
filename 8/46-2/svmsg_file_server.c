#include "svmsg_file.h"

#include <errno.h>  // for errno
#include <stdlib.h> // for EXIT_FAILURE
#include <signal.h> // for SA_RESTART
#include <string.h>
#include <stdio.h>
#include <unistd.h> // for read

static void grimReaper(int sig) {
    int savedErrno;

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

// Executed in child process: serve a single client
static void serveRequest(const struct requestMsg *req, int sum) {
    struct responseMsg resp;
    
    resp.mtype = RESP_MT_DATA;
    resp.seqNum = sum;
    msgsnd(req->clientId, &resp, sizeof(resp.seqNum), 0);
}

int main(int argc, char *argv[]) {
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    int serverId;
    struct sigaction sa;
    int sum = 0;

    /* Create server message queue */

    serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if (errno == EEXIST) {
        // The queue already exists; open it instead of creating a new one
        serverId = msgget(SERVER_KEY, 0);
    } else {
        perror("msgget");
        return 1;
    }

    /* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    /* Read requests, handle each in a separate child process */

    for (;;) {
        msgLen = msgrcv(serverId, &req, sizeof(req) - sizeof(long), 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            printf("msgrcv\n");           /* Some other error */
            break;                      /* ... so terminate loop */
        }

        pid = fork();                   /* Create child process */
        if (pid == -1) {
            printf("fork\n");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
            serveRequest(&req, sum);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
        sum += req.seqLen;
    }

    /* If msgrcv() or fork() fails, remove server MQ and exit */

    if (msgctl(serverId, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return 1;
    }
    exit(EXIT_SUCCESS);
}
