/*
 * is_background.c :  check for & at end
 */


#include <stdio.h>
#include <string.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.
  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 *
	 * Fill in code.
	 */

	// i is the string count of myArgv
	int i = 0;
	while(myArgv[i] != NULL) {
		i++;
	}

	// '&' should be the last element in myArgv
	if (strcmp(myArgv[i-1], "&") == 0) {
		myArgv[i-1] = NULL;	// replace '&' with NULL
		return TRUE;
	}

    return FALSE;

}