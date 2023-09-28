/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"




/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_echo(char **argv) {
  	/* Fill in code. */
	if(strcmp(argv[1], "-n") == 0) {
		int num = atoi(argv[2]);	// return 0 on error
		if(num == 0) {
			printf("Error! echo -n usage example: echo -n <number> ...\n");
		} else if(argv[num+2] == NULL) {
			printf("Error! argv doen't have %d elements after \"echo -n %d\"\n", num, num);
		} else {
			printf("%s\n", argv[num+2]);
		}
	} else {
		int i = 1;
		while (argv[i]!= NULL) {
			printf("%s ", argv[i++]);
		}
		printf("\n");
	}
}
/* Fill in code. */
static void bi_exit(char **argv) {
	exit(0);
}




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {

	/* Fill in code. */

	{ "echo", bi_echo },		/* When "echo" is typed, bi_echo() executes.  */
	{ "quit", bi_exit },		// When "quit" is typed, bi_exit()
	{ "exit", bi_exit },		// When "exit" is typed, bi_exit()
	{ "logout", bi_exit },		// When "logout" is typed, bi_exit()
	{ "bye", bi_exit },			// When "bye" is typed, bi_exit()
	{ NULL, NULL }				/* NULL terminated. */
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
}
