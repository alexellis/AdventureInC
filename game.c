#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lib/types.h"
#include "lib/loader.h"


void showHelp() {
	printf(
"================================================================================\n"
"Help\n"
"================================================================================\n"
" (l)ook\t\tinspect environment, or item\n"
" (u)p\t\tmove up\n"
" (d)own\t\tmove down\n"
" quit\t\texit the game\n"
" (h)elp\t\tshow this screen\n"
    );
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

int execute_command(struct player * me, char * cmd_buffer, char * cmd_param) {
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
			me->currentRoom = nextRoom;
		} else {
			printf("You didn't move.\n");
		}
	}
	else if(strcmp(cmd_buffer, "look")==0) {
		if(cmd_param!=NULL &&strlen(cmd_param) > 0) {
			printf("You do not notice anything special about: %s.\n", cmd_param);
		}
		else {
			look_player(me);
		}
	}
	else if(strcmp(cmd_buffer, "help")==0) { 
		showHelp();
	}
	else if(strcmp(cmd_buffer, "quit")==0) {
		ret = 1;
	}
	else {
		char expanded[30];
		if(find_alias(&expanded, cmd_buffer, me)) {
			printf("Using alias %s.\n", expanded);
			int childRet = execute_command(me, &expanded, cmd_param);
			ret = childRet;

		} else {
			printf("I do not understand: '%s'.. ?\n", cmd_buffer);
		}
	}
	return ret;
}

void loop_player(struct player * me) {
	char cmd_buffer[80];
	char cmd_param[80];
	while(1) {
		printf("> ");
		memset(&cmd_buffer, '\0',sizeof(char)*80);
		if(fgets(cmd_buffer, 80, stdin)) {
			trimEnd(&cmd_buffer, '\n');

			// parse argument
			// 'look me' ->
			// ' me'
			char * spacep = strstr(&cmd_buffer, " ");
			char * cmd_buffer_p;
			if(spacep) {
				memset(&cmd_param, '\0',sizeof(char)*80);

				// ' me'
				strcpy(&cmd_param, spacep);

				// 'look me'
				cmd_buffer_p=&cmd_buffer;

				while(cmd_buffer_p != spacep) {
					cmd_buffer_p++;
				}
				// 'look me' -> 'look'
				*cmd_buffer_p='\0';

				// ' me' -> 'me'
				spacep++;
				cmd_buffer_p=spacep;
			} else {
				memset(&cmd_param, '\0',sizeof(char)*80);
				cmd_buffer_p=&cmd_param;
			}
			int ret = execute_command(me, (char*)&cmd_buffer, cmd_buffer_p);
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

void initPlayer(struct player * me) {
	me->aliases = (void*)NULL;
	memset(me->name, '\0', sizeof(me->name));
}


int main(void) {
	const char * path = "start";
	struct room *current = read_room(path);

	struct player me;
	initPlayer(&me);
	read_player(&me);
	push_alias(&me, "l", "look");
	push_alias(&me, "u", "up");
	push_alias(&me, "d", "down");
	push_alias(&me, "lo", "look");	// Just an extra alias
	push_alias(&me, "h", "help");


	me.currentRoom = current;
	printf("Welcome: %s\n",me.name);
	look_player(&me);
	loop_player(&me);
}
