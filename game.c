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
" look me\tinformation on current player\n"
" (ex)amine\texamine surroundings\n"
" (u)p\t\tmove up\n"
" (d)own\t\tmove down\n"
" quit\t\texit the game\n"
" (h)elp\t\tshow this screen\n"
    );
}

struct commandResult {
  int exit;
  int moved;
};

void print_room(struct room * current) {
    printf("%s\n%s\n", current->name, current->desc);

    struct connection *link = current->link;
    printf("Exits: -\n");
    while(link) {
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

int find_alias(char * expanded, char * cmd_buffer, struct player * me) {
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
    time(&me->connectionTime);
}

void look_player(struct player * me) {
    print_room(me->currentRoom);
}

struct commandResult execute_command(struct player * me, char * cmd_buffer, char * cmd_param) {
    struct commandResult result;
    int ret = 0;
    int moved = 0;

    if(strlen(cmd_buffer) == 0) {
        printf("What?\n");
    }
    else if(has_exit(me->currentRoom, cmd_buffer)) {
        printf("You go: %s\n", cmd_buffer);
        char room_file[30];
        memset(&room_file, '\0', sizeof(char)*30);
        get_exit(&room_file, me->currentRoom, cmd_buffer);
        //free currentRoom..

        struct room * nextRoom = read_room(room_file);
        if(nextRoom != NULL) {
            // Also free links
            // free(me->currentRoom);
            me->currentRoom = nextRoom;
            moved = 1;
        } else {
            printf("You didn't move.\n");
        }
    }
    else if(strcmp(cmd_buffer, "examine") == 0) {
      if((me->currentRoom)->items == NULL) {
        printf("You do not notice anything.\n");
      } else {
        printf("You notice:-\n");
        struct item * itemPtr = (me->currentRoom)->items;
          do {
            printf("%s\n", itemPtr->name);
            itemPtr = itemPtr->link;
          } while(itemPtr != NULL);
      }
    }
    else if(strcmp(cmd_buffer, "look") == 0) {
        if(cmd_param != NULL && strlen(cmd_param) > 0) {
    //      printf("%s\n", cmd_param);
            if(strcmp(cmd_param, "me") == 0) {
                time_t now;
                time(&now);
                int seconds = (int)difftime(now, me->connectionTime);

                printf(
"================================================================================\n"
"Player: %s\n"
"================================================================================\n"
"Current room: %s\n"
"Connected for: %d seconds\n",
                me->name, (me->currentRoom)->name, seconds);
            } else {
                printf("You do not notice anything special about: '%s'.\n", cmd_param);
            }
        }
        else {
            look_player(me);
        }
    }
    else if(strcmp(cmd_buffer, "help") == 0) {
        showHelp();
    }
    else if(strcmp(cmd_buffer, "quit")==0) {
        ret = 1;
    }
    else {
        char expanded[30];
        if(find_alias(&expanded, cmd_buffer, me)) {
            printf("Using alias %s.\n", expanded);
            struct commandResult childResult = execute_command(me, &expanded, cmd_param);
            ret = childResult.exit;
            moved = childResult.moved;

        } else {
            printf("I do not understand: '%s'.. ?\n", cmd_buffer);
        }
    }
    result.exit = ret;
    result.moved = moved;
    return result;
}

int parse_parameter(char* cmd_buffer, char* cmd_param) {
    int parsedParam = 0;
    char * spacep = strstr(cmd_buffer, " ");
    char * cmd_buffer_p = cmd_buffer;
    if(spacep) {
        while(cmd_buffer_p != spacep) {
            cmd_buffer_p++;
        }
        // 'look me' -> 'look'
        *cmd_buffer_p='\0';

        spacep++;
        // ' me'
        strcpy(cmd_param, spacep);
        parsedParam = 1;
    }
}

void loop_player(struct player * me) {
    char cmd_buffer[80];
    char cmd_param[80];

    while(1) {
        printf(PROMPT);
        memset(&cmd_buffer, '\0', sizeof(char)*80);
        memset(&cmd_param, '\0', sizeof(char)*80);

        if(fgets(cmd_buffer, 80, stdin)) {
            trimEnd(&cmd_buffer, '\n');

            int parsed = parse_parameter(&cmd_buffer, &cmd_param);
            struct commandResult result = execute_command(me, (char*)&cmd_buffer, cmd_param);
            if(result.moved) {
                memset(&cmd_param, '\0', sizeof(char)*80);
                result = execute_command(me, (char*)&"look", cmd_param);
            }

            if(result.exit) {
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
    push_alias(&me, "lo", "look");  // Just an extra alias
    push_alias(&me, "h", "help");
    push_alias(&me, "ex", "examine");

    me.currentRoom = current;
    printf("Welcome: %s\n",me.name);
    look_player(&me);
    loop_player(&me);
}
