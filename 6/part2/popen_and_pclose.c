#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>  // for errno

#define MAX_FDS 20

typedef struct {
    FILE    *stream;
    pid_t   pid;
} ChildInfo;

static ChildInfo childInfo[MAX_FDS] = {0};

FILE *my_popen(const char *command, const char *type) {
    int     fds[2];
    pid_t   pid;
    FILE    *stream;

    if( (*type != 'r' && *type != 'w') || type[1]) {
        return NULL;
    }

    // create pipe
    if(pipe(fds) == -1) {
        return NULL;
    }

    pid = fork();
    if( pid == -1) {
        close(fds[0]);
        close(fds[1]);
        return NULL;
    }

    // child process
    if(pid == 0) {
        if(*type == 'r') {
            close(fds[0]);  // close read

            // set write to STDOUT
            if(fds[1] != STDOUT_FILENO) {
                dup2(fds[1], STDOUT_FILENO);
                close(fds[1]);
            }

        } else if(*type == 'w') {
            close(fds[1]);  // close write

            // set read to STDIN
            if(fds[0] != STDIN_FILENO) {
                dup2(fds[0], STDIN_FILENO);
                close(fds[0]);
            }
        }

        /*
            /bin/sh:     shell path
            sh:          assign to shell
            -c:          command to execute
            (char *)0:   the end of argument list
        */
        execl("/bin/sh", "sh", "-c", command, (char *)0);

        // if execl fails, exit with error
        // 127 means that the executed program/command was not found.
        _exit(127);
    }

    // parent process
    else if(pid > 0) {
        if(*type == 'r') {
            close(fds[1]);  // close write

            // associate read with pipe
            stream = fdopen(fds[0], type);
            if(stream == NULL) {
                return NULL;
            }
        } else if(*type == 'w') {
            close(fds[0]);  // close read

            // associate write with pipe
            stream = fdopen(fds[1], type);
            if(stream == NULL) {
                return NULL;
            }
        }

        // fd stands for the index
        int fd = fileno(stream);
        if(fd < 0 || fd >= MAX_FDS) {
            fclose(stream);
            return NULL;
        }

        childInfo[fd].stream = stream;
        childInfo[fd].pid = pid;
    }
    return stream;
}

int my_pclose(FILE *stream) {
    int fd = fileno(stream);

    if(fd < 0 || fd >= MAX_FDS || childInfo[fd].stream != stream) {
        return -1;
    }

    pid_t pid = childInfo[fd].pid;
    childInfo[fd].stream = NULL;
    childInfo[fd].pid = 0;

    fclose(stream);

    int status;
    while(waitpid(pid, &status, 0) == -1) {
        
        // If error is not caused by interruption of waitpid, set status to -1.
        if(errno != EINTR) {    //EINTR represents an interrupt signal
            status = -1;
            break;
        }
    }
    return status;
}

int main() {
    FILE *stream;
    char buffer[1024];

    /* version 1 */
    stream = my_popen("cat > example.txt", "w");
    if (stream != NULL) {
        fputs("Good morning.\n", stream);
        fputs("You're the most handsome TA.\n", stream);
        fputs("Please let me pass.\n", stream);
        my_pclose(stream);
    } else {
        perror("my_popen");
        exit(EXIT_FAILURE);
    }

    stream = my_popen("cat example.txt", "r");
    if(stream) {
        while(fgets(buffer, sizeof(buffer), stream) != NULL) {
            printf("%s", buffer);
        }
        my_pclose(stream);
    } else {
        perror("my_popen");
        exit(EXIT_FAILURE);
    }

    /* version 2*/
    // stream = my_popen("ls -l", "r");

    // if(stream) {
    //     while(fgets(buffer, sizeof(buffer), stream) != NULL) {
    //         printf("%s", buffer);
    //     }
    //     my_pclose(stream);
    // } else {
    //     perror("my_popen");
    //     exit(EXIT_FAILURE);
    // }

    return 0;
}
