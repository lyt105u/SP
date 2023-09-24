#include <unistd.h>     // lseek
#include <fcntl.h>      // open
#include <sys/stat.h>   // mode
#include <errno.h>
#include <stdio.h>
#include <getopt.h>     // getopt
#include <stdlib.h>     // atoi

#define BUF_SIZE 100


int main(int argc, char *argv[]) {
    int opt;
    int fd;
    int num = 10;
    int newLinesRead = 0;
    char buf[BUF_SIZE];
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
                num = atoi(optarg);
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

    // set the offset to the end of the file
    // if lseek failed, returns -1
    if ((fileSize = lseek(fd, 0, SEEK_END)) == 0) {
        perror("lseek error: ");
        return 0;
    }

    while(fileSize) {
        if(fileSize <= BUF_SIZE) {
            lseek(fd, 0, SEEK_SET);
        } else {
            fileSize -= BUF_SIZE;
            lseek(fd, -BUF_SIZE, SEEK_CUR);
        }
        if(read(fd, buf, BUF_SIZE) > 0) {
            for(int i=BUF_SIZE-1; i >= 0; i--) {
                if(buf[i] == '\n') {
                    newLinesRead++;
                    // printf("*%d*", newLinesRead);
                    if(newLinesRead == num) {
                        offset = i;
                        break;
                    }
                }
                lseek(fd, -BUF_SIZE, SEEK_CUR);
                // printf("%c", buf[i]);
            }
        }
        if(newLinesRead == num) {
            break;
        }
    }

    lseek(fd, -offset, SEEK_CUR);
    while(read(fd, buf, BUF_SIZE) > 0) {
        write(STDOUT_FILENO, buf, BUF_SIZE);
    }

    // close file
    if(close(fd) == -1) {
        perror("Close file error: ");
        return 0;
    }

    return 0;
}
