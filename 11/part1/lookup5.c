/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "dict.h"

#define MAX_WORD_LENGTH 32
#define MAX_DEFINITION_LENGTH 480

int lookup(char * buffer, const char * resource) {
	FILE *file;
	char word[MAX_WORD_LENGTH];
    char definition[MAX_DEFINITION_LENGTH];
    int ch, i;

	file = fopen(resource, "r");
    if (file == NULL) {
        perror("Error in opening file");
        return(-1);
    }

	while (!feof(file)) {
        // Read word
        i = 0;
        while ((ch = fgetc(file)) != '\0' && ch != EOF && i < MAX_WORD_LENGTH - 1) {
            word[i++] = ch;
        }
        word[i] = '\0';

        // Skip null characters
        while ((ch = fgetc(file)) == '\0' && ch != EOF);

        // Read definition
        i = 0;
        if (ch != EOF) {
            ungetc(ch, file); // Put back the last non-null character read
            while ((ch = fgetc(file)) != '\n' && ch != EOF && i < MAX_DEFINITION_LENGTH - 1) {
                definition[i++] = ch;
            }
            definition[i] = '\0';
        }


        // Check if the word matches
        if (strcmp(word, buffer) == 0) {
			strcpy(buffer, definition);
            fclose(file);
            return FOUND;
        }

        // Skip remaining null characters before next word
        while ((ch = fgetc(file)) == '\0' && ch != EOF);
        if (ch != EOF) {
            ungetc(ch, file); // Reset for reading the next word
        }
    }
    
	strcpy(buffer, "XXXX");
	fclose(file);
	return NOTFOUND;
}
