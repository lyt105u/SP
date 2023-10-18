#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h> // for gethostname(), gethostid()

int main() {
    char hostname[128];
    struct utsname sysInfo;

    if(gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return 1;
    }
    printf("hostname: %s\n", hostname);

    if(uname(&sysInfo) != 0) {
        perror("uname");
        return 1;
    }
    printf("%s\n", sysInfo.release);

    printf("hostid: %ld\n", gethostid());
    
    return 0;
}