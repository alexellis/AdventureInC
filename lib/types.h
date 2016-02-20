#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAX_ROOM_DESC 160*3
#define PROMPT "> "


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
	char fileName[200];
	char name[150];
	char desc[MAX_ROOM_DESC];
	struct connection *link;
	struct item *items;
};

struct LoadedRoom {
	struct room* current;
	struct LoadedRoom* next;
};

typedef struct LoadedRoom LoadedRoom;

typedef struct {
	struct player* main;
	const char * name;
	LoadedRoom *rooms;
} World;

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
	World *world;
};


int trimEnd(char *line, char end);
