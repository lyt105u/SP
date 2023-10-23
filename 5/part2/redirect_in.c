/*
 * redirect_in.c  :  check for <
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0
#include <stdio.h>
#include <string.h>

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.	
 */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd;

  	/* search forward for <
  	 *
	 * Fill in code. */
	while(myArgv[i] != NULL) {
		if(strcmp(myArgv[i], "<") == 0) {
			break;
		}
		i++;
	}

  	if (myArgv[i]) {	/* found "<" in vector. */
		char *filename = myArgv[i + 1];

		// open file
		fd = open(filename, O_RDONLY);
        if (fd < 0) {
            perror("open");
            return -1;
        }

		// redirect stdin to use the file for input
        if(dup2(fd, STD_INPUT) < 0) {
            perror("dup2");
            return -1;
        }

		// close file descriptor and clean up
        close(fd);

		// remove "<" and the filename from myArgv
		myArgv[i] = NULL;
  	}
  	return 0;
}
