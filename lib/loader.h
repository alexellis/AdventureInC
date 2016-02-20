#define UNSET -1
#define NORMAL 0
#define ATTRIB 1
#define COMMENT 2

#define PROMPT	"> "
#define DIRECTION_SEP	"="

int getLineType(const char * ptr);

void strip(const char * ptr, char * currentSection, int max);

void read_connection(struct connection * next, char * line);

void walk_connections(struct connection * pt);

struct room* read_room(const char * path);

FILE* openRoomFile( const char * path);

World* new_world(const char * name);
