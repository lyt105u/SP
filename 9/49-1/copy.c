#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void errExit(char *err) {
    perror(err);
    return;
}

int main(int argc, char *argv[]) {
    char *src, *dst;
    int srcFd, dstFd;
    struct stat sb;

    if (argc != 3) {
        printf("Usage: %s <src_file> <dst_file>\n", argv[0]);
        return 1;
    }

    srcFd = open(argv[1], O_RDONLY);
    if (srcFd == -1)
        errExit("open");

    // get file size, and store in sb
    if (fstat(srcFd, &sb) == -1)
        errExit("fstat");

    // if empty src
    if (sb.st_size == 0) {
        printf("The src_file is empty\n");
        return 0;
    }

    // src is mapped for reading
    src = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if (src == MAP_FAILED)
        errExit("mmap");

    dstFd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (dstFd == -1)
        errExit("open");

    // set dst file size
    if (ftruncate(dstFd, sb.st_size) == -1)
        errExit("ftruncate");

    // dst is mapped for reading and writing
    dst = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, dstFd, 0);
    if (dst == MAP_FAILED)
        errExit("mmap");

    // copy
    memcpy(dst, src, sb.st_size);

    // sync memory-mapped area
    if (msync(dst, sb.st_size, MS_SYNC) == -1)
        errExit("msync");

    return 0;
}
