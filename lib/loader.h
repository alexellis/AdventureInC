int getLineType(const char*ptr);

void strip(const char * ptr, char* currentSection, int max);

void read_connection(struct connection * next, char * line);

void walk_connections(struct connection * pt);

struct room* read_room(const char * path);