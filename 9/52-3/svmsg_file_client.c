#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

#define SERVER_QUEUE_NAME   "/server_queue"
#define CLIENT_QUEUE_NAME   "/client_queue"
#define QUEUE_PERMISSIONS   0660
#define MAX_MESSAGES        10
#define MAX_MSG_SIZE        512
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)

int main(int argc, char *argv[]) {
    mqd_t qd_server, qd_client;   // Queue descriptors
    struct mq_attr attr;
    char in_buffer[MSG_BUFFER_SIZE];
    char out_buffer[MSG_BUFFER_SIZE];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s <pathname>\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > MSG_BUFFER_SIZE - 1) {
        printf("<pathname> too long (max: %d bytes)\n", MSG_BUFFER_SIZE - 1);
        return 1;
    }

    // Create the client queue for receiving messages from server
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    qd_client = mq_open(CLIENT_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (qd_client == -1) {
        perror("Client: mq_open (client)");
        exit(1);
    }
    
    // Open the server queue for sending messages
    qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (qd_server == -1) {
        perror("Client: mq_open (server)");
        exit(1);
    }

    // Send message to server
    snprintf(out_buffer, MSG_BUFFER_SIZE, "%s", argv[1]);
    if (mq_send(qd_server, out_buffer, strlen(out_buffer) + 1, 0) == -1) {
        perror("Client: mq_send");
        exit(1);
    }
    
    // Receive the response from the server
    if (mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
        perror("Client: mq_receive");
        exit(1);
    }

    printf("Client: message received from server:\n");
    if(strncmp(in_buffer, "Couldn't open", sizeof("Couldn't open"))) {
        printf("%s\n", in_buffer);
    } else {
        printf("%ld bytes\n", strlen(in_buffer));
    }

    // Cleanup
    mq_close(qd_client);
    mq_unlink(CLIENT_QUEUE_NAME);
    mq_close(qd_server);

    exit(EXIT_SUCCESS);
}
