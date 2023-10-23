/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL || strcmp(line,"\n")==0) {
    	return NULL;
  	}

  	// Init strtok with commandline, then get first token.
	// Return NULL if no tokens in line.
	token = strtok(line, delim);
	if(token == NULL) {
        return NULL;
    }

  	// Create array with room for first token.
	newArgv = (char **)malloc(sizeof(char *));
    if(newArgv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

	// Give the first token its own memory, then install it.
	newArgv[count] = (char *)malloc(strlen(token) + 1);
    if(newArgv[count] == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
	strcpy(newArgv[count++], token);

	// While there are more tokens...
	token = strtok(NULL, delim);
	while(token != NULL) {
		// Resize array.
        newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
        if(newArgv == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

		// Give token its own memory, then install it.
		if(token != NULL) {
            newArgv[count] = (char *)malloc(strlen(token) + 1);
            if(newArgv[count] == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strcpy(newArgv[count++], token);
        }

		// Get next token.
		token = strtok(NULL, delim);
	}


  	// Null terminate the array and return it.
	newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
    if(newArgv == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    newArgv[count] = NULL;

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */
}
