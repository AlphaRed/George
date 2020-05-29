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
    ZIPPO,      // nothing
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
    ITEM0,      // O2 tank
    ITEM1,      // snorkel
    ITEM2,      // brochure
    ITEM3,      // windmill
    ITEM4,      // bedspring/mattress
    ITEM5,      // accordian
    NPC0,       // scientist
    NPC1,       // welder
    NPC2,       // glider pilot
    NPC3,       // hairdresser
    NPC4,       // diver
    NPC5,       // windmill operator
    NPC6,       // street vendor
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
    LEVEL1,     // mansion
    LEVEL2,     // generator room
    LEVEL3,     // street
    LEVEL4,     // windmill - beside geysers
    LEVEL5,     // car repair shop
    LEVEL6,     // hair dressers
    LEVEL7,     // cliff
    LEVEL8,     // pond
    LEVEL9      // dump
} CurrLevel;

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES]);
int loadLevel(char* filename, int array[MAP_HEIGHT][MAP_WIDTH]);

#endif
