#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // for getopt

void ussageMsg();

int main(int argc, char *argv[]) {
    int opt;
    char *mode = "w", c;
    FILE *fp;

    if(argc < 2) {
        printf("Error: invalid argument\n");
        ussageMsg();
        return 0;
    }

    // option
    if( (opt = getopt(argc, argv, "a")) != -1 ) {
        switch(opt) {
            case 'a':
                mode = "a";
                break;
            default:
                return 0;
        }
    }

    // open file
    if( !(fp = fopen(argv[argc-1], mode)) ) {
        perror("Open error: ");
        ussageMsg();
        return 0;
    }

    // read stdin
    while( (c=fgetc(stdin))!=EOF ) {
        // write file
        if( fputc(c, fp) == EOF ) {
            perror("Write error: ");
        }
        // stdout
        printf("%c", c);
    }

    // close file
    if( fclose(fp) == EOF ) {
        perror("Close error: ");
    }

    return 0;
}

void ussageMsg() {
    printf("Usage example: ls -l | ./tee [-a] file.txt\n");
}
