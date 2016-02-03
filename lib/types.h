#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAX_ROOM_DESC 160*3
#define PROMPT "> "

struct alias {
	char src[30];
	char dest[30];
	struct alias * next;
};

struct player {
	struct room * currentRoom;
	char name [55];
	struct alias * aliases;
	time_t connectionTime;
};

struct connection {
	char direction[30];
	char file[30];
	struct connection * next;
};

struct item {
	char name[50];
	char description[100];
	struct item *link;
};

struct room {
	char name[150];
	char desc[MAX_ROOM_DESC];
	struct connection *link;
	struct item *items;
};

int trimEnd(char *line, char end);
