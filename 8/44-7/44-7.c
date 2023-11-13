#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MESSAGE "Good morning, this is FIFO!"

int main() {
    // check if FIFO already exists
    if (access("myfifo", F_OK) == -1) {
        // create FIFO
        if (mkfifo("myfifo", 0666) == -1) {
            perror("mkfifo");
            return 1;
        }
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    // child (writer)
    if (pid == 0) {
        // Open FIFO
        int fd = open("myfifo", O_WRONLY | O_NONBLOCK);

        if (fd == -1) {
            perror("open (writing)");
            return 1;
        }

        // Write to FIFO
        ssize_t bytes_written = write(fd, MESSAGE, sizeof(MESSAGE) - 1);

        if (bytes_written == -1) {
            perror("write");
        } else {
            printf("Child wrote %zd bytes to FIFO.\n", bytes_written);
        }

        close(fd);

    }
    
    // parent (reader)
    else {
        // Open FIFO
        int fd = open("myfifo", O_RDONLY | O_NONBLOCK);

        if (fd == -1) {
            perror("open (read)");
            return 1;
        }

        printf("Parent opened FIFO in nonblocking mode.\n");

        // Wait for the child process to finish
        wait(NULL);

        // Read from FIFO
        char buffer[256];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

        if (bytes_read == -1) {
            perror("read");
        } else {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            printf("Parent read from FIFO: %s\n", buffer);
        }

        close(fd);

        // unlink FIFO
        if (unlink("myfifo") == -1) {
            perror("Error unlinking FIFO");
        }
    }

    return 0;
}