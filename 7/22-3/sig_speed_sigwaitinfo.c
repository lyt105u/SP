#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define TESTSIG SIGUSR1

int main(int argc, char *argv[]) {
    sigset_t blockedMask;
    pid_t childPid;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s num-sigs\n", argv[0]);
        return 1;
    }

    int numSigs = atoi(argv[1]);

    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    childPid = fork();
    switch (childPid) {
        case -1:
            perror("fork");
            exit(1);

        case 0:     /* child */
            for (int scnt = 0; scnt < numSigs; scnt++) {
                if (kill(getppid(), TESTSIG) == -1) {
                    perror("kill");
                    exit(1);
                }

                if (sigwaitinfo(&blockedMask, NULL) == -1 && errno != EINTR) {
                    perror("sigwaitinfo");
                    exit(1);
                }
            }
            exit(EXIT_SUCCESS);

        default: /* parent */
            for (int scnt = 0; scnt < numSigs; scnt++) {
                if (sigwaitinfo(&blockedMask, NULL) == -1 && errno != EINTR) {
                    perror("sigwaitinfo");
                    exit(1);
                }
                if (kill(childPid, TESTSIG) == -1) {
                    perror("kill");
                    exit(1);
                }
            }
            exit(EXIT_SUCCESS);
        }
}