/*
 * The same main will do to test all the versions  of  lookup.   The
 * argument  to  main  is  always  passed  as the second argument to
 * lookup. It identifies the resource needed by lookup to  find  its
 * answers.  It  might be a file name or a named pipe, or the key to
 * some shared memory or a message queue, etc. The point is,  it  is
 * just  a  string;  main does not need to understand how the string
 * will be used. The work involved in each  exercise  is  to  define
 * lookup in different ways. What is the same throughout is that the
 * FIRST time lookup is called, it may have to open some resource. A
 * static internal flag is used to determine whether or not this was
 * the first call.
 */

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "dict.h"

#define BUFFER_SIZE 480
#define WORD_SIZE 32

int main(int argc, char **argv) {
	char word[WORD_SIZE];
	char buffer[BUFFER_SIZE];

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <resource>\n",argv[0]);
		exit(errno);
	}

	// while(fputs("What word do you want : ",stderr),gets(tryit.word)) {
	while (fputs("What word do you want : ", stderr), fgets(word, sizeof(word), stdin) != NULL) {
		word[strcspn(word, "\n")] = '\0';
		strcpy(buffer, word);
		switch(lookup(buffer, argv[1]) ) {
			case FOUND:
				printf("%s : %s\n", word, buffer);
				break;
			case NOTFOUND:
				printf("%s : Not Found!\n", word) ;
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
	}
}