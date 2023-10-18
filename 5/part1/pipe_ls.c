#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>   // for wait()

int main() {
    // fd[0] is read, fd[1] is write
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if(pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // child process
        close(fd[0]);

        close(STDOUT_FILENO);
        dup(fd[1]);

        close(fd[1]);
        execl("/bin/ls", "ls", "-l", (char *)0);
    } else {
        // parent process
        close(fd[1]);
        wait(NULL);

        // read pipe
        char buffer[1024];
        while(read(fd[0], buffer, sizeof(buffer)) != 0) {
            printf("%s", buffer);
        }
        close(fd[0]);
    }
    return 0;
}
