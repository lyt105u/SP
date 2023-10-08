#include <stdio.h>
#include <dirent.h> // for DIR
#include <unistd.h> // for ssize_t
#include <string.h> // for strcmp
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

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    char filepath[BUFSIZE];
    DIR *fdDir;
    struct dirent *fdEntry;
    ssize_t len;
    char buf[1024];
    int pid;
    FILE *fp;
    char line[BUFSIZE];
    char *title;
	char *value;
    char proc_name[BUFSIZE];
    char proc_id[BUFSIZE];

    // invalid arguements
    if(argc != 2) {
        printf("Usage: ./listPathOpen <filepath>\n");
        return 1;
    }

    dir = opendir("/proc");
    if(dir == NULL) {
        perror("opendir");
        return 1;
    }

    while( (entry=readdir(dir)) != NULL ) {
        if(entry->d_type == DT_DIR) {
            // filepath = "/proc/<pid>/fd"
            strcpy(filepath, "/proc/");
			strcat(filepath, entry->d_name);
            strcat(filepath, "/fd");

            // check whether fdDir has an fd subdir
            fdDir = opendir(filepath);
            if (fdDir == NULL) {
                continue;
            }

            while( (fdEntry=readdir(fdDir)) != NULL) {
                if (fdEntry->d_type == DT_LNK) {
                    // filepath = "/proc/<pid>/fd/<fd>"
                    strcat(filepath, "/");
                    strcat(filepath, fdEntry->d_name);
                    pid = atoi(fdEntry->d_name);

                    len = readlink(filepath, buf, sizeof(buf) - 1);

                    // if fdDir has subdir
                    if (len != -1) {
                        buf[len] = '\0';

                        // match argv[1]
                        if (strcmp(buf, argv[1]) == 0) {
                            // filepath =  "/proc/<pid>/status"
                            strcpy(filepath, "/proc/");
                            strcat(filepath, entry->d_name);
                            strcat(filepath, "/status");
                            fp = fopen(filepath, "r");
                            if (fp == NULL) {
                                continue;
                            }

                            // get the name and id
                            while(fgets(line, sizeof(line), fp)) {
                                title = strtok(line, ":");
                                value = strtok(NULL, ":");
                                if(strcmp(title, "Name") == 0) {
                                    strcpy(proc_name, trim(value));
                                } else if(strcmp(title, "Pid") == 0) {
                                    strcpy(proc_id, trim(value));
                                }
                            }
                            fclose(fp);

                            printf("Process %d has file (%s %s) open at %s\n", pid, proc_id, proc_name, argv[1]);
                        }
                    }
                }
            }
            closedir(fdDir);
        }
    }

    closedir(dir);
    return 0;
}