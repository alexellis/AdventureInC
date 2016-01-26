#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define UNSET -1
#define NORMAL 0
#define ATTRIB 1
#define COMMENT 2

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

struct room {
	char name[150];
	char desc[160*3];
	struct connection *link;
};

int trimEnd(char *line, char end);