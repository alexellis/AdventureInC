#include "types.h"
#include "loader.h"
#define ROOM_EXT_NAME ".txt"


World* new_world(const char * name) {
	World *w=malloc(sizeof(World));
	w->name=name;
	w->rooms = NULL;
	return w;
}

void init_connection(struct connection* pt);

FILE* openRoomFile(const char * path) {
	FILE* fp;
	char * extPath = malloc(sizeof(char) * strlen(path) +(strlen(ROOM_EXT_NAME) + 1));
	strcpy(extPath, path);
	strcat(extPath, ".txt");

	fp = fopen(extPath, "rb");
	if(fp == 0) {
	 	return fp;
	}
	free(extPath);
	extPath = NULL;
	return fp;
}

void init_item(struct item * current) {
	memset(current->name, '\0', 50);
	memset(current->description, '\0', 100);
	current->link = NULL;
}

void init_room(struct room * current) {
	memset(current->desc, '\0', 160*3);
	memset(current->name, '\0', 150);
	memset(current->fileName, '\0', 200);

	current->link = NULL;
}

void read_item(struct item * next, char * line) {
	strcpy(next->name, strtok(line, DIRECTION_SEP));
	strcpy(next->description, strtok(NULL, DIRECTION_SEP));
}

void processItem(struct room * current, char * line) {
	struct item * itemPtr=current->items;
	if(itemPtr == NULL) {
		current->items = (struct item*) malloc (sizeof(struct item));
		itemPtr = current->items;
	} else {
		while(itemPtr->link != NULL) {
			itemPtr = itemPtr->link;
		}
		itemPtr->link = (struct item*) malloc (sizeof(struct item));
		itemPtr = itemPtr->link;
	}
	init_item(itemPtr);
	read_item(itemPtr,line);
}

struct room* read_room(const char * path) {
	FILE* fp = openRoomFile(path);

	if(fp == NULL) {
		return NULL;
	}

	struct room* current = (struct room*) malloc ( sizeof(struct room) );
	init_room(current);
	memcpy(current->fileName, path,200);

	char currentSection[35];

	char line[160];
	int lastLineType = UNSET;
	int lineType = UNSET;

	while(!feof(fp)) {
		char *ptr = (char*)&line;
		int lineRead = fgets(ptr, 160, fp);
		if(!lineRead) {
			break;
		}
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
				int lineLen=strlen(line);
				assert(descLen + lineLen <= MAX_ROOM_DESC);

				char * dptr=current->desc;
				for(int i=0; i< descLen; i++){
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
				 	ptr = ptr->next;
				 }
				int headPopulated = strlen(ptr->file);
				read_connection(ptr, line);
			} else if(strcmp(currentSection, "items") == 0) {
				processItem(current, line);
			}
		}
		lastLineType=lineType;
	}
	fclose(fp);
	return current;
}

int getLineType(const char * ptr) {
	if(*ptr == '[') {
		return ATTRIB;
	} else if(*ptr == '#') {
		return COMMENT;
	}

	return NORMAL;
}

void strip(const char * ptr, char * currentSection, int max) {
	char *walker = currentSection;
	for(int i = 0; i< max; i++) {
		*walker = '\0';
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
	strcpy(next->direction, strtok(line, DIRECTION_SEP));
	strcpy(next->file, strtok(NULL, DIRECTION_SEP));
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
