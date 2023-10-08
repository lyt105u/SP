#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>    // for pwd from userIdFromName()
#include <stdlib.h> // for strtol()
#include <dirent.h> // for DIR
#include <unistd.h>
#include <string.h>
#include <ctype.h>  // for isspace()

#define BUFSIZE 256

/* Return UID corresponding to 'name', or -1 on error */
uid_t userIdFromName(const char *name) {
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */
        return -1;                      /* return an error */

    u = strtol(name, &endptr, 10);      /* As a convenience to caller */
    if (*endptr == '\0')                /* allow a numeric string */
        return u;

    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;

    return pwd->pw_uid;
}

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
    const char *username;
    uid_t id;
    char id_str[BUFSIZE];
    DIR *dir;
    struct dirent *entry;
    char filepath[BUFSIZE];
    FILE *fp;
    char line[BUFSIZE];
    char *title;
	char *value;
    char proc_name[BUFSIZE];
    char proc_id[BUFSIZE];
    int print_flag;

    if(argc != 2) {
        printf("Error! Usage: ./listProc <username>\n");
        return -1;
    }

    username = argv[1];
    id = userIdFromName(username);
    if(id == (uid_t)-1) {
        printf("Error! User %s does not exist.\n", username);
        return -1;
    }

    // transfer data type of process id from int to string
    sprintf(id_str, "%d", id);

    dir = opendir("/proc");
    if(dir == NULL) {
        perror("Open /proc directory error");
        return -1;
    }

    while( (entry=readdir(dir)) != NULL ) {
        if( ((entry->d_type==DT_DIR) || (entry->d_type==DT_LNK)) && atoi(entry->d_name) != 0) {
            // filepath = "/proc/<pid>/status"
            strcpy(filepath, "/proc/");
			strcat(filepath, entry->d_name);
            strcat(filepath, "/status");

            // open /proc/<pid>/status
            fp = fopen(filepath, "r");
            print_flag = 0;

            // if /proc/<pid>/status disappears, skip this process
            if (fp == NULL) {
                continue;
            }

            while(fgets(line, sizeof(line), fp)) {
                title = strtok(line, ":");
		        value = strtok(NULL, ":");

                if( (strcmp(title, "Uid")==0) && (strstr(value, id_str)!=NULL) ) {
                    // set print flag
                    print_flag = 1;
                } else if(strcmp(title, "Name") == 0) {
                    strcpy(proc_name, value);
                } else if(strcmp(title, "Pid") == 0) {
                    strcpy(proc_id, value);
                }
            }
        
            fclose(fp);
        }

        // print results if flag is set
        if(print_flag == 1) {
            strcpy(proc_id, trim(proc_id));
            strcpy(proc_name, trim(proc_name));
            printf("%s: %s\n", proc_id, proc_name);
        }
    }

    closedir(dir);
    return 0;
}