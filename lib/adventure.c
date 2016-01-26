#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int trimEnd(char *line, char end){
	char * ptr=(char*)line;
	while(*ptr!='\0'){
		ptr++;
	}

	int newLine = line[strlen(line)-1] == end;
	if(newLine) {
		line[strlen(line)-1]='\0';
	} else {
		return -1;
	}
	return 0;
}