#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * trim char 'end' from 'line'
 * i.e. pass end='\n' to strip a newline from a string.
 *
 * returns 0 if trim performed, -1 if not.
 */
int trimEnd(char *line, char end) {
	// char * ptr=(char*)line;
	// while(*ptr!='\0') {
	// 	printf("%s\n", ptr);
	// 	ptr++;
	// }

	int newLine = line[strlen(line)-1] == end;
	if(newLine) {
		line[strlen(line)-1]='\0';
	} else {
		return -1;
	}
	return 0;
}