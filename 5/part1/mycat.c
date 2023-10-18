#include <stdio.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: ./mycat <filename>\n");
        return 1;
    }

    FILE *fp;
    char ch;

    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        printf("%s does not exist.\n", argv[1]);
        return 1;
    }

    while ((ch = fgetc(fp)) != EOF) {
        printf("%c", ch);
    }

    fclose(fp);
    return 0;
}
