/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;
	char line[BIGLINE];
	static Dictrec dr/*, blank*/;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	// while (!feof(in)) {
	while(fgets(line, BIGLINE, in) != NULL) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */
		line[strcspn(line, "\n")] = 0;	// remove newline character
        strncpy(dr.word, line, WORD);	// Read word and put in record
        dr.word[WORD - 1] = '\0';		// Truncate at the end of the "word" field

		strcpy(dr.text, ""); // initialize
        while( (fgets(line,BIGLINE,in)!=NULL) && (strcmp(line,"\n")!=0) ) {	// possibly multiple lines
            strcat(dr.text, line);
        }
		dr.text[strcspn(dr.text, "\n")] = 0;	// remove newline character

		// Pad unused characters with nulls
		for (int i = strlen(dr.text); i < TEXT; i++) {
            dr.text[i] = '\0';
        }

		// Write record out to file
		fwrite(&dr, sizeof(Dictrec), 1, out);
	}

	fclose(in);
	fclose(out);
	return 0;
}
