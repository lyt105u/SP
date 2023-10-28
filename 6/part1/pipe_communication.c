#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main() {
    int     fd1[2];
    int     fd2[2];
    pid_t   pid;
    char    inBuf[BUFFER_SIZE];
    char    outBuf[BUFFER_SIZE];

    if(pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if(pid < 0) {
        perror("fork");
        exit(1);
    }

    // parent process
    if(pid > 0) {
        close(fd1[0]);  // close the read of the first pipe
        close(fd2[1]);  // close the write of the second pipe

        while(1) {
            memset(inBuf, 0, BUFFER_SIZE);  // Clear the inBuf
            
            printf("Input text: ");
            fgets(inBuf, BUFFER_SIZE, stdin);
            write(fd1[1], inBuf, strlen(inBuf) + 1);
            read(fd2[0], outBuf, BUFFER_SIZE);
            printf("Parent responses: %s", outBuf);
        }
    } else {  // child process
        close(fd2[0]);  // close the read end of the second pipe
        close(fd1[1]);  // close the write end of the first pipe

        while(1) {
            read(fd1[0], inBuf, BUFFER_SIZE);

            memset(outBuf, 0, BUFFER_SIZE); // Clear the outBuf

            // Convert to uppercase
            for(int i = 0; inBuf[i] != '\0'; i++) {
                outBuf[i] = toupper(inBuf[i]);
            }

            write(fd2[1], outBuf, strlen(outBuf) + 1);
        }
    }

    return 0;
}
