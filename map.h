#ifndef MAP_H
#define MAP_H

#define FILE_LVL1   "1.lvl"
#define FILE_LVL2   "2.lvl"

#define FILE_ENT1   "1.ent"
#define FILE_ENT2   "2.ent"

#define MAX_ENTITIES    25

#define MAP_WIDTH       20
#define MAP_HEIGHT      15

typedef enum
{
    ZIPPO,
    EXIT1,
    EXIT2,
    EXIT3,
    EXIT4,
    EXIT5,
    EXIT6,
    EXIT7,
    EXIT8,
    EXIT9,
    EXIT10,
    EXIT11,
    EXIT12,
    EXIT13,
    EXIT14,
    EXIT15,
    ITEM0,
    ITEM1,
    NPC0,
    NPC1,
    NPC2
} entityType;

struct entity
{
    int x;
    int y;
    entityType type;
} entities[MAX_ENTITIES];

int lvl[MAP_HEIGHT][MAP_WIDTH];

enum
{
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4,
    LEVEL5,
    LEVEL6,
    LEVEL7,
    LEVEL8
} CurrLevel;

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES]);
int loadLevel(char* filename, int array[MAP_HEIGHT][MAP_WIDTH]);

#endif
