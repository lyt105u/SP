#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    long size;
    char *buf;
    char *cwd;

    size = pathconf(".", _PC_PATH_MAX);
    if((buf = (char *)malloc((size_t)size)) != NULL) {
        cwd = getcwd(buf, (size_t)size);
        if(cwd != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
            exit(1);
        }
        free(buf);
    } else {
        perror("malloc");
        exit(1);
    }

    return 0;
}
