#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define UNSET -1
#define NORMAL 0
#define ATTRIB 1

struct alias {
	char src[30];
	char dest[30];
	struct alias * next;
};

struct player {
	struct room * currentRoom;
	char name [55];
	struct alias * aliases; 
};

struct connection {
	char direction[30];
	char file[30];
	struct connection * next;
};

struct room {
	char name[150];
	char desc[160*3];
	struct connection *link;
};

int trimEnd(char *line, char end){
	char * ptr=(char*)line;
	while(*ptr!='\0'){
		ptr++;
	}

	int newLine = line[strlen(line)-1] == end;
	if(newLine) {
		line[strlen(line)-1]='\0';
	}else {
		return -1;
	}
	return 0;
}

int getLineType(const char*ptr){

	if(*ptr=='['){
		return ATTRIB;
	}

	return NORMAL;
}

void strip(const char * ptr, char* currentSection, int max) {
	char *walker=currentSection;
	for(int i=0;i<max;i++){
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

void walk_connections(struct connection * pt){
	while(pt->next){
		pt=pt->next;
	}
}


struct room* read_room(const char * path) {
	struct room* current = (struct room*) malloc ( sizeof(struct room) );
	memset(current->desc, '\0', 160*3);
	memset(current->name, '\0', 150);

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
				 	ptr = current->link;
				}
				else {
					ptr = current->link;
				 	walk_connections(ptr);
				 	ptr->next = (struct connection*)malloc(sizeof(struct connection));
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

void print_room(struct room * current) {
	printf("%s\n%s\n", current->name, current->desc);

	struct connection *link = current->link;
	printf("Exits: -\n");
	while(link){
		printf(" %s ", link->direction);
		link = link->next;
	}
	printf("\n");
}

int get_exit(char * filePath, struct room * current, const char * to) {
	struct connection * head = current->link;
	int found = 0;
	do {
		int isMatch = strcmp(head->direction, to)==0;
		if(isMatch) {
			strcpy(filePath, head->file);
			found = 1;
			break;
		}
		head=head->next;
	} while(head && strlen(head->file)>0);
	return found;
}

int has_exit(struct room * current, const char * to) {
	struct connection * head = current->link;
	int found = 0;
	do {
		int isMatch = strcmp(head->direction, to)==0;
		if(isMatch){
			found = 1;
			break;
		}
		head=head->next;
	} while(head && strlen(head->file)>0);
	return found;
}

int find_alias(char * expanded, char * cmd_buffer ,struct player * me) {
	struct alias * head = me->aliases;
	int found = 0;
	do {
		int isMatch = strcmp(head->src, cmd_buffer)==0;
		if(isMatch) {
			strcpy(expanded, head->dest);
			found = 1;
			break;
		}
		head=head->next;
	} while(head!=NULL && strlen(head->src)>0);

	return found;
}

void read_player(struct player * me) {
	printf("Welcome to the game.\nPlayer name: ");
	fgets(me->name, 55, stdin);
	char * ch = me->name;
	while(*ch != '\0'){
		ch++;
	}
	ch--;
	if(*ch=='\n') {
		*ch='\0';
	}
}

void look_player(struct player * me) {
	print_room(me->currentRoom);
}

int execute_command(struct player * me, char * cmd_buffer) {
	int ret=0;
	if(strlen(cmd_buffer) == 0) {
		printf("What?\n");
	}
	else if(has_exit(me->currentRoom, cmd_buffer)) {
		printf("You go: %s\n", cmd_buffer);
		char room_file[30];
		get_exit((char*)&room_file, me->currentRoom, cmd_buffer);
		//free currentRoom..

		struct room * nextRoom = read_room(room_file);
		if(nextRoom != NULL) {
			// Also free links
			// free(me->currentRoom);
			me->currentRoom = nextRoom ;
		} else {
			printf("You didn't move.\n");
		}
	}
	else if(strcmp(cmd_buffer, "look")==0){
		look_player(me);
	}
	else if(strcmp(cmd_buffer, "quit")==0){
		ret = 1;
	}
	else {
		char expanded[30];
		if(find_alias(&expanded, cmd_buffer, me)){
			printf("Using alias %s.\n", expanded);
			int childRet = execute_command(me, &expanded);
			ret= childRet;

		} else {
			printf("I do not understand: '%s'.. ?\n", cmd_buffer);
		}
	}
	return ret;
}

void loop_player(struct player * me) {
	char cmd_buffer[80];

	while(1) {
		printf("> ");
		if(fgets(cmd_buffer, 80, stdin)) {
			trimEnd(&cmd_buffer, '\n');

			int ret = execute_command(me, (char*)&cmd_buffer);
			if(ret){
				break;
			}
		}else {
			break;
		}
	}
	printf("OK... Bye.\n");
}

void initalizeAlias(struct alias * item){
	memset(item->src, '\0', sizeof(char)*30);
	memset(item->dest, '\0', sizeof(char)*30);
	item->next=NULL;
}

void push_alias(struct player *me, const char * src, const char * dest) {
	if(me->aliases == NULL) {
		struct alias * aliases = (struct alias*)malloc(sizeof(struct alias));
		initalizeAlias(aliases);
		me->aliases=aliases;
	}

	struct alias * head = me->aliases;
	do {

		if(strlen ( head->src ) == 0) {
			break;
		} else if(head->next !=NULL) {
			head = head->next;
		} else {
			struct alias * nextItem = (struct alias*)malloc(sizeof(struct alias));
			initalizeAlias(nextItem);
			head->next = nextItem;
			head = head->next;
			
		}
	} while(1);

	if(head!=NULL) {
		strcpy(head->src, src);
		strcpy(head->dest, dest);
	}
}

int main(void) {
	const char * path="start";
	struct room *current = read_room(path);

	struct player me;
	me.aliases = (void*)NULL;
	read_player(&me);
	push_alias(&me, "l", "look");
	push_alias(&me, "u", "up");
	push_alias(&me, "d", "down");
	push_alias(&me, "lo", "look");	// Just an extra alias


	me.currentRoom = current;
	printf("Welcome: %s\n",me.name);
	look_player(&me);
	loop_player(&me);
}
