#include <unistd.h>     // lseek
#include <fcntl.h>      // open
#include <sys/stat.h>   // mode
#include <errno.h>
#include <stdio.h>
#include <getopt.h>     // getopt
#include <stdlib.h>     // atoi

#define BUF_SIZE 100

void printCurrentLocation(char *str, int fd) {
    printf("%s, ", str);
    printf("current location: %ld\n", lseek(fd, 0, SEEK_CUR));
}

int main(int argc, char *argv[]) {
    int opt;
    int fd;
    int num = 10;
    int newLinesRead = 0;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE];
    int fileSize;
    int offset = 0;

    if (argc!=2 && argc!=4) {
        printf("Error!\n");
        printf("Usage example: ./tail [-n num] file.txt\n");
        return 0;
    }

    // check option
    // if no options detected, returns -1
    if( (opt = getopt(argc, argv, "n:")) != -1 ) {
        switch(opt) {
            case 'n':
                num = atoi(optarg) + 1;
                printf("num: %d\n", num);
                break;          
            default:
                return 0;
        }
    }

    // open file
    fd = open(argv[argc-1], O_RDONLY);
    if(fd < 0) {
        perror("Open file error: ");
        return 0;
    }

    // move file pointer to the end of the file
    // if lseek failed, returns -1
    if ((fileSize = lseek(fd, 0, SEEK_END)) == -1) {
        perror("lseek error1: ");
        return 0;
    }
    printCurrentLocation("after file size", fd);

    while(1) {
        // move file pointer
        if(fileSize <= BUF_SIZE) {
            // move file pointer to the front
            if(lseek(fd, 0, SEEK_SET) == -1) {
                perror("lseek error2: ");
                return 0;
            }
            printCurrentLocation("file size not enouge", fd);
        } else {
            if(lseek(fd, -BUF_SIZE, SEEK_CUR) == -1) {
                perror("lseek error3: ");
                return 0;
            }
            printCurrentLocation("after - BUF_SIZE", fd);
        }
        fileSize -= BUF_SIZE;

        // read file
        if(read(fd, buf, BUF_SIZE) > 0) {
            for(int i=BUF_SIZE-1; i >= 0; i--) {
                if(buf[i] == '\n') {
                    newLinesRead++;
                    if(newLinesRead == num) {
                        offset = i;
                        printf("offset: %d\n", offset);
                        break;
                    }
                }
            }
            printCurrentLocation("after read", fd);
        }

        // move file pointer backward after reading
        if(offset != 0) {
            if(lseek(fd, -offset-1, SEEK_CUR) == -1) {
                perror("lseek error4: ");
                return 0;
            }
            printCurrentLocation("have offset", fd);
            break;
        } else if(fileSize <= 0) {
            if(lseek(fd, 0, SEEK_SET) == -1) {
                perror("lseek error5: ");
                return 0;
            }
            printCurrentLocation("not enough file", fd);
            break;
        } else {
            if(lseek(fd, -BUF_SIZE, SEEK_CUR) == -1) {
                perror("lseek error6: ");
                return 0;
            }
            printCurrentLocation("keep moving", fd);
        }
    }
    while(read(fd, buf2, BUF_SIZE) > 0) {
        fflush(stdout);
        write(1, buf2, BUF_SIZE);
    }

    // close file
    if(close(fd) == -1) {
        perror("Close file error: ");
        return 0;
    }

    return 0;
}
