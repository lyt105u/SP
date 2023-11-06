#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);

void handler(int sig) {
    printf("Signal %d handled.\n", sig);
}

sighandler_t my_sigset(int sig, sighandler_t disp) {
    struct sigaction newact, oldact;
    newact.sa_handler = disp;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    if (sigaction(sig, &newact, &oldact) < 0)
        return SIG_ERR;
    return oldact.sa_handler;
}

void my_sighold(int sig) {
    printf("sighold()\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void my_sigrelse(int sig) {
    printf("sigrelse()\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void my_sigignore(int sig) {
    printf("sigignore()\n");
    my_sigset(sig, SIG_IGN);
}

void cancel_sigignore(int sig) {
    printf("cancel sigignore()\n");
    my_sigset(SIGINT, handler);
}

void my_sigpause() {
    printf("sigpause()\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigsuspend(&mask);
}

int main() {
    my_sigset(SIGINT, handler);

    printf("Hold for 5 seconds, and the release.\n");
    my_sighold(SIGINT);
    sleep(5);
    kill(getpid(), SIGINT); // got hold till sigrelse()
    my_sigrelse(SIGINT);
    kill(getpid(), SIGINT);
    printf("\n");

    sleep(1);

    printf("Ignore for 5 seconds, and then cancel it.\n");
    my_sigignore(SIGINT);
    kill(getpid(), SIGINT); // ignored
    sleep(5);
    kill(getpid(), SIGINT); // ignored
    cancel_sigignore(SIGINT);
    kill(getpid(), SIGINT);
    printf("\n");

    sleep(1);

    printf("Pause. Use ^c to unpause.\n");
    my_sigpause();
    kill(getpid(), SIGINT);

    return 0;
}
