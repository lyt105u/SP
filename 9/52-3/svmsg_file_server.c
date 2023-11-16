// #include "svmsg_file.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mqueue.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

#include <signal.h>

#define SERVER_QUEUE_NAME   "/server_queue"
#define CLIENT_QUEUE_NAME   "/client_queue"
#define QUEUE_PERMISSIONS   0660
#define MAX_MESSAGES        10
#define MAX_MSG_SIZE        512
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)

mqd_t qd_server; // queue descriptor

// handler for Ctrl+C or Ctrl+Z
void handler(int sig) {
    printf("\nCaught signal SIGINT (Ctrl+C or Ctrl+Z)\n");
    mq_close(qd_server);
    mq_unlink(SERVER_QUEUE_NAME);
    exit(0);
}

// send to client
static void serveRequest(char *in_buf) {
    mqd_t qd_client;
    int fd;
    char out_buf[MSG_BUFFER_SIZE] = {0};
    ssize_t numRead;

    // Open the client queue
    qd_client = mq_open(CLIENT_QUEUE_NAME, O_WRONLY);
    if (qd_client == -1) {
        perror("Server: Not able to open client queue");
        return;
    }

    fd = open(in_buf, O_RDONLY);
    if (fd == -1) {
        snprintf(out_buf, sizeof(out_buf), "Couldn't open %s", in_buf);
        
        // Send response message to client
        if (mq_send(qd_client, out_buf, strlen(out_buf) + 1, 0) == -1) {
            perror("Server: mq_send");
        }
        exit(EXIT_FAILURE);
    }

    while ((numRead = read(fd, out_buf, MSG_BUFFER_SIZE)) > 0) {
        if (mq_send(qd_client, out_buf, strlen(out_buf) + 1, 0) == -1) {
            perror("Server: mq_send");
        }
    }
}

int main(int argc, char *argv[]) {
    pid_t pid;
    struct mq_attr attr;
    char in_buf[MSG_BUFFER_SIZE] = {0};

    // Register signal handlers
    signal(SIGINT, handler);    // Ctrl+C
    signal(SIGTSTP, handler);   // Ctrl+Z
    
    // Create the server queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (qd_server == -1) {
        perror("Server: mq_open (server)");
        exit(1);
    }
    
    /* Read requests, handle each in a separate child process */
    for (;;) {
        // Get the next message from the queue
        if (mq_receive(qd_server, in_buf, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("Server: mq_receive");
            continue;
        }
        printf("Server: message received: %s\n", in_buf);

        pid = fork();
        if (pid == -1) {
            perror("fork");
            break;
        }

        if (pid == 0) {
            // child handles request
            serveRequest(in_buf);
            _exit(EXIT_SUCCESS);
        }
    }

    mq_close(qd_server);
    mq_unlink(SERVER_QUEUE_NAME);
    exit(EXIT_SUCCESS);
}
