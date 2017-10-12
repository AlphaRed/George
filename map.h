#ifndef MAP_H
#define MAP_H

#define MAX_ENTITIES    20

struct entity
{
    int x;
    int y;
    int type;
} entities[MAX_ENTITIES];

int lvl[21][21];

enum
{
    Level1,
    Level2
} CurrLevel;

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES]);
int loadLevel(char* filename, int array[21][21]);

#endif
