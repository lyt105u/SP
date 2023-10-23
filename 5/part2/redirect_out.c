/*
 * redirect_out.c   :   check for >
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
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd;

  	/* search forward for >
  	 * Fill in code. */
	while(myArgv[i] != NULL) {
		if(strcmp(myArgv[i], ">") == 0) {
			break;
		}
		i++;
	}

  	if (myArgv[i]) {	/* found ">" in vector. */
		// open file
		fd = open(myArgv[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open");
            return -1;
        }

		// redirect to use fd for output
		if (dup2(fd, STD_OUTPUT) < 0) {
            perror("dup2");
            return -1;
        }

		// close file descriptor and clean up
		close(fd);

		// remove ">" and the filename from myArgv
		myArgv[i] = NULL;
  	}
  	return 0;
}
