#include "svmsg_file.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int clientId;

static void
removeQueue(void) {
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return;
    }
}

int main(int argc, char *argv[]) {
    struct requestMsg req;
    struct responseMsg resp;
    int serverId;
    ssize_t msgLen;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s [seq-len]\n", argv[0]);
        return 1;
    }
        
    /* Get server's queue identifier; create queue for response */

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
        perror("msgget - server message queue");
        return 1;
    }

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) {
        perror("msgget - client message queue");
        return 1;
    }

    if (atexit(removeQueue) != 0) {
        perror("atexit");
        return 1;
    }

    /* Send message asking for file named in argv[1] */

    req.mtype = 1;                      /* Any type will do */
    req.clientId = clientId;
    req.seqLen = atoi(argv[1]);

    if (msgsnd(serverId, &req, sizeof(req) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        return 1;
    }

    /* Get first response, which may be failure notification */

    msgLen = msgrcv(clientId, &resp, sizeof(resp), 0, 0);
    if (msgLen == -1) {
        perror("msgrcv");
        return 1;
    }

    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}
