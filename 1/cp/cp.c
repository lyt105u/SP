#include <unistd.h>     // lseek
#include <fcntl.h>      // open
#include <sys/stat.h>   // mode
#include <errno.h>
#include <stdio.h>

void ussageMsg();

int main(int argc, char *argv[]) {
    int input, output, element_cnt;
    char buf[100];
    mode_t mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWOTH);    // fix permission denied problem
    off_t last_byte;

    if(argc != 3) {
        printf("Error: invalid argument\n");
        ussageMsg();
        return 0;
    }

    // open input file
    input = open(argv[1], O_RDONLY);
    if(input < 0) {
        perror("Open input file error: ");
        ussageMsg();
        return 0;
    }

    // open output file
    output = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, mode);
    if(output < 0) {
        perror("Open output file error: ");
        ussageMsg();
        return 0;
    }

    // read input file block by block
    while( (element_cnt = read(input, buf, 100)) > 0) {
        for(int i=0; i<element_cnt; i++) {
            if(buf[i] == '\0') {
                // read hole, offset + 1
                lseek(output, 1, SEEK_CUR);
            } else {
                // write 1 byte from &buf[i] into output file
                write(output, &buf[i], 1);
            }
        }
    }

    // set the offset of output file to the end
    last_byte = lseek(input, -1, SEEK_END);
	lseek(output, last_byte, SEEK_SET);
	write(output, "", 1);

    // close file
    if( (close(input)||close(output)) == -1) {
        perror("Close file error: ");
        return 0;
    }

    return 0;
}

void ussageMsg() {
    printf("Usage example: ./cp <input_file> <output_file>\n");
}
