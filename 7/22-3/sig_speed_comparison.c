#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>  // for pid_t
#include <unistd.h>     // for fork()
#include <time.h>

static void handler(int sig) {}

#define TESTSIG SIGUSR1

void sig_speed_sigsuspend(int numSigs) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(TESTSIG, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }
    sigset_t blockedMask, emptyMask;
    pid_t childPid;

    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
        perror("sigprocmask");
        return;
    }
    sigemptyset(&emptyMask);

    childPid = fork();
    switch (childPid) {
        case -1:
            perror("fork");
            return;

        case 0:     /* child */
            for (int scnt = 0; scnt < numSigs; scnt++) {
                if (kill(getppid(), TESTSIG) == -1) {
                    perror("kill");
                    return;
                }
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                    perror("sigsuspend");
                    return;
                }
            }
            exit(EXIT_SUCCESS);

        default: /* parent */
            for (int scnt = 0; scnt < numSigs; scnt++) {
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
                    perror("siguspend");
                    return;
                }
                if (kill(childPid, TESTSIG) == -1) {
                    perror("kill");
                    return;
                }
            }
            return;
    }
}

void sig_speed_sigwaitinfo(int numSigs) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(TESTSIG, &sa, NULL) == -1) {
        perror("sigaction");
        return;
    }

    sigset_t emptyMask;
    sigemptyset(&emptyMask);

    pid_t childPid = fork();
    switch (childPid) {
    case -1:
        perror("fork");
        return;

    case 0:     /* child */
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1) {
                perror("kill");
                return;
            }

            siginfo_t si;
            if (sigwaitinfo(&emptyMask, &si) == -1) {
                perror("sigwaitinfo");
                return;
            }
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (int scnt = 0; scnt < numSigs; scnt++) {
            siginfo_t si;
            if (sigwaitinfo(&emptyMask, &si) == -1) {
                perror("sigwaitinfo");
                return;
            }

            if (kill(childPid, TESTSIG) == -1) {
                perror("kill");
                return;
            }
        }
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{
    int numSigs;
    struct timespec start, end;
    double elapsed;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s num-sigs\n", argv[0]);
        return 1;
    }

    numSigs = atoi(argv[1]);
    if(numSigs == 0) {
        printf("Invalid num-sugs\n");
        return 1;
    }

    // struct sigaction sa;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sa.sa_handler = handler;
    // if (sigaction(TESTSIG, &sa, NULL) == -1) {
    //     perror("sigaction");
    //     return 1;
    // }

    // sigsuspend
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // sig_speed_sigsuspend(numSigs);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; // 總執行時間，以秒為單位
    // printf("執行時間: %f 秒\n", elapsed);

    // sigwaitinfo
    clock_gettime(CLOCK_MONOTONIC, &start);
    sig_speed_sigwaitinfo(numSigs);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; // 總執行時間，以秒為單位
    printf("執行時間: %f 秒\n", elapsed);
}