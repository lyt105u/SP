#include <stdio.h>
#include <dirent.h> // for DIR
#include <string.h> // for strcpy
#include <stdlib.h> // for atoi
#include <ctype.h>  // for isspace

#define BUFSIZE 256

char *trim(char *str) {
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end))
    end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void printTree(const char *ppid, int depth) {
    DIR *dir;
    struct dirent *entry;
    char filepath[BUFSIZE];
    FILE *fp;
    char line[BUFSIZE];
    char *title;
	char *value;
    int printFlag;
    char proc_name[BUFSIZE];
    char proc_id[BUFSIZE];

    dir = opendir("/proc");
    if(dir == NULL) {
        perror("opendir");
        return;
    }

    while( (entry=readdir(dir)) != NULL ) {
        printFlag = 0;
        if( ((entry->d_type==DT_DIR) || (entry->d_type==DT_LNK)) && atoi(entry->d_name) != 0) {
            // filepath = "/proc/<pid>/status"
            strcpy(filepath, "/proc/");
			strcat(filepath, entry->d_name);
            strcat(filepath, "/status");

            // open /proc/<pid>/status
            fp = fopen(filepath, "r");
            if (fp == NULL) {
                continue;
            }

            while(fgets(line, sizeof(line), fp)) {
                title = strtok(line, ":");
		        value = strtok(NULL, ":");
                // compare with the PPid
                if((strcmp(title, "PPid")==0) && (strcmp(trim(value), ppid)==0)) {
                    printFlag = 1;
                } else if(strcmp(title, "Name") == 0) {
                    strcpy(proc_name, trim(value));
                } else if(strcmp(title, "Pid") == 0) {
                    strcpy(proc_id, trim(value));
                }
            }
            fclose(fp);

            if(printFlag == 1) {
                for(int i=0; i<depth; i++) {
                    printf("  ");
                }
                printf("|-%s\n", trim(proc_name));
                printTree(trim(proc_id), depth + 1);
            }
        }
    }
    closedir(dir);
}

int main() {
    printTree("0", 0);
    return 0;
}