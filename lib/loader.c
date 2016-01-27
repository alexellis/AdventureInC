#include "types.h"
#include "loader.h"

const char* prompt = "> ";


int getLineType(const char * ptr) {

	if(*ptr == '[') {
		return ATTRIB;
	} else if(*ptr == '#') {
		return COMMENT;
	}

	return NORMAL;
}

void strip(const char * ptr, char* currentSection, int max) {
	char *walker=currentSection;
	for(int i=0;i<max;i++) {
		*walker='\0';
		walker++;
	}
	walker = currentSection;

	int index = 0;
	while(*ptr != '\0') {
		if(index) {
			*currentSection = *ptr;
			currentSection++;
		}
		ptr++;
		index++;
	}
	*(--currentSection)='\0';
}

void read_connection(struct connection * next, char * line) {
	char sep[2] = "=";
	strcpy(next->direction, strtok(line,sep));
	strcpy(next->file, strtok(NULL,sep));
}

void walk_connections(struct connection * pt) {
	while(pt->next!=NULL) {
		pt = pt->next;
	}
}

void init_connection(struct connection* pt) {
	memset(pt->file, '\0', 30);
	memset(pt->direction, '\0', 30);
	pt->next = NULL;
}

struct room* read_room(const char * path) {
	struct room* current = (struct room*) malloc ( sizeof(struct room) );
	memset(current->desc, '\0', 160*3);
	memset(current->name, '\0', 150);
	current->link = NULL;

	char * extPath = malloc( sizeof(char) * strlen(path) +(strlen(".txt") + 1) );
	strcpy(extPath, path);
	strcat(extPath, ".txt");

	FILE* fp;
	fp = fopen(extPath, "rb");
	if(fp ==0){

	 	return NULL;
	}
	free(extPath);
	extPath=NULL;

	char currentSection[35];

	char line[160];
	int lastLineType = -1;
	int lineType = -1;

	while(!feof(fp)) {
		char *ptr = (char*)&line;
		fgets(ptr, 160, fp);
		trimEnd(ptr, '\n');

		lineType = getLineType(ptr);
		if(lineType == COMMENT) {
			continue;
		}
		if(lineType == ATTRIB) {
			strip(ptr, (char*)&currentSection, 35);
			//printf("Section: %s\n", currentSection);
		} else {
			if(strcmp(currentSection, "name") == 0) {
				strcpy(current->name, line);
			} else if(strcmp(currentSection, "desc") == 0) {
				int descLen = strlen(current->desc);
				char * dptr=current->desc;
				for(int i=0;i<descLen;i++){
					dptr++;
				}

				memcpy(dptr, line, strlen(line));
			} else if(strcmp(currentSection, "exits") == 0) {
				struct connection  * ptr;

				if(!current->link) {
				 	current->link = (struct connection*)malloc(sizeof(struct connection));
				 	init_connection(current->link);
				 	ptr = current->link;
				}
				else {
					ptr = current->link;
				 	walk_connections(ptr);
				 	ptr->next = (struct connection*)malloc(sizeof(struct connection));
				 	init_connection(ptr->next);
				 	ptr=ptr->next;
				 }

				int headPopulated = strlen(ptr->file);
				read_connection(ptr, line);
			}
		}

		//printf("lineType=[%d] %s\n", lineType, line);
		lastLineType=lineType;
	}
	fclose(fp);
	return current;
}