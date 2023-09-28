/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int stat;
    int background = FALSE;

    /* Create a new child process.
     * Fill in code.
	 */
    if(is_background(myArgv)) {
        background = TRUE;
    }
    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
            int status;
            if(!background) {
                waitpid(pid, &status, 0);
            }

            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
			 */
            if(WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);       
                printf("Exit status of the child was %d\n", exit_status);
            }

            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
            int status_code = execvp(myArgv[0], myArgv);
            /* Handle error return from exec */
            if(status_code == -1) {
                perror("Execvp error");
                exit(errno);
            }
    }
}
