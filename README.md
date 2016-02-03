## AdventureInC

### Description

This is a text-based adventure game written in C as a learning exercise.

### Changelog

- Library added and separate Makefile for parsing code
- Makefile added
- Aliases added for player commands
- Separation of parsing and executing commands
- Loading and parsing a room text file
- Added start.txt and upper\_vantage.txt rooms.

### Build and running

If you have a gcc toolchain, it should be as simple as:

```
make
./game
```

#### Room format

Rooms are of the format:

```
[name]
Short name max length: 150 chars
[desc]
Multi-line is
OK, but max length is 160*3 chars.
[extra]
item1,alias1=item1 description
item2=item2 description
[exits]
direction=file_name_without_extension
```

**[desc]**

Multi-line is OK.

**[extra]**

Not implemented

**[exit]**

Alias on left, do not include .txt on right-hand side.

Then enter your player name and have a look around.

================================

Alex Ellis 2016
