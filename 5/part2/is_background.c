/*
 * is_background.c :  check for & at end
 */

#include <stdio.h>
#include "shell.h"
#include <string.h>

int is_background(char ** myArgv) {
  	if (*myArgv == NULL)
    	return 0;

	int i = 0;

	// "&" should be the last element
	while(myArgv[i] != NULL) {
		if(strcmp(myArgv[i], "&") == 0) {
			myArgv[i-1] = NULL;	// replace '&' with NULL
			return TRUE;	// "&" found
		}
		i++;
	}

	return FALSE;	// "&" not found
}