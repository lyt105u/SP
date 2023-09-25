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
    int optNum = 10;
    int newLineCnt = 0;
    char buf[BUF_SIZE];
    int fileSize;
    int oriFileSize;
    int offset = 0;
    int outputLen = 0;

    if (argc!=2 && argc!=4) {
        printf("Argument error!\n");
        printf("Usage example: ./tail [-n optNum] file.txt\n");
        return 0;
    }

    // check option
    // if no options detected, returns -1
    if( (opt = getopt(argc, argv, "n:")) != -1 ) {
        switch(opt) {
            case 'n':
                optNum = atoi(optarg);
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

    // move file pointer to the end
    if ((fileSize = lseek(fd, 0, SEEK_END)) == -1) {
        perror("lseek error(1): ");
        return 0;
    }
    oriFileSize = fileSize;

    while(1) {
        // buf size is larger than the remaining file size, which haven't been read yet
        if(fileSize <= BUF_SIZE) {
            // move file pointer to the front
            if(lseek(fd, 0, SEEK_SET) == -1) {
                perror("lseek error(2): ");
                return 0;
            }
        } else {
            // move file pointer backward for BUF_SIZE bytes
            if(lseek(fd, -BUF_SIZE, SEEK_CUR) == -1) {
                perror("lseek error(3): ");
                return 0;
            }
        }
        fileSize -= BUF_SIZE;

        // read file
        if(read(fd, buf, BUF_SIZE) > 0) {
            int range = 0;
            if(oriFileSize < BUF_SIZE) {
                range = oriFileSize;
            } else {
                range = BUF_SIZE;
            }
            for(int i=range-1; i >= 0; i--) {
                if(buf[i] == '\n') {
                    newLineCnt++;
                    if(newLineCnt == optNum+1) {    // file contains extra '\n' in the end
                        offset = i;
                        break;
                    }
                }
            }
        }

        // collect enough '\n'
        // move file pointer to the character after the last '\n'
        if(offset != 0) {
            if(oriFileSize < BUF_SIZE) {
                if(lseek(fd, -(oriFileSize-offset-1), SEEK_CUR) == -1) {
                    perror("lseek error(4): ");
                    return 0;
                }
            } else {
                if(lseek(fd, -(BUF_SIZE-offset-1), SEEK_CUR) == -1) {
                    perror("lseek error(4_1): ");
                    return 0;
                }
            }
            break;
        } else if(fileSize <= 0) {
            // file doesn't have enough '\n'
            // move file pointer to the front
            if(lseek(fd, 0, SEEK_SET) == -1) {
                perror("lseek error(5): ");
                return 0;
            }
            break;
        } else {
            // not enough '\n', so keep reading
            // move file pointer backward for BUF_SIZE bytes
            if(lseek(fd, -BUF_SIZE, SEEK_CUR) == -1) {
                perror("lseek error(6): ");
                return 0;
            }
        }
    }

    // output result
    while((outputLen=read(fd, buf, BUF_SIZE)) > 0) {
        fflush(stdout);
        if(write(1, buf, outputLen) == -1) {
            perror("Write output error: ");
            return 0;
        }
    }

    // close file
    if(close(fd) == -1) {
        perror("Close file error: ");
        return 0;
    }

    return 0;
}
