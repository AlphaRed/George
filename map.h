#ifndef MAP_H
#define MAP_H

#define FILE_LVL1   "1.lvl"
#define FILE_LVL2   "2.lvl"
#define FILE_LVL3   "3.lvl"
#define FILE_LVL4   "4.lvl"
#define FILE_LVL5   "5.lvl"
#define FILE_LVL6   "6.lvl"
#define FILE_LVL7   "7.lvl"
#define FILE_LVL8   "8.lvl"
#define FILE_LVL9   "9.lvl"

#define FILE_ENT1   "1.ent"
#define FILE_ENT2   "2.ent"
#define FILE_ENT3   "3.ent"
#define FILE_ENT4   "4.ent"
#define FILE_ENT5   "5.ent"
#define FILE_ENT6   "6.ent"
#define FILE_ENT7   "7.ent"
#define FILE_ENT8   "8.ent"
#define FILE_ENT9   "9.ent"

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
    EXIT16,
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
    NPC7        // trash monster
} entityType;

#define ITEM_O2             ITEM0
#define ITEM_SNORKEL        ITEM1
#define ITEM_BROCHURE       ITEM2
#define ITEM_WINDMILL       ITEM3
#define ITEM_MATTRESS       ITEM4
#define ITEM_ACCORDION      ITEM5

#define NPC_SCIENTIST       NPC0
#define NPC_WELDER          NPC1
#define NPC_PILOT           NPC2
#define NPC_HAIRDRESSER     NPC3
#define NPC_DIVER           NPC4
#define NPC_WINDMILLOP      NPC5
#define NPC_VENDOR          NPC6
#define NPC_TRASHMONSTER    NPC7

extern struct entity
{
    int x;
    int y;
    entityType type;
} entities[MAX_ENTITIES];

extern int lvl[MAP_HEIGHT][MAP_WIDTH];

typedef enum
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
} Level;
extern Level CurrLevel;

#define LEVEL_MANSION           LEVEL1
#define LEVEL_GENERATOR         LEVEL2
#define LEVEL_STREET            LEVEL3
#define LEVEL_WINDMILL          LEVEL4
#define LEVEL_CARREPAIR         LEVEL5
#define LEVEL_HAIRDRESSERS      LEVEL6
#define LEVEL_CLIFF             LEVEL7
#define LEVEL_POND              LEVEL8
#define LEVEL_DUMP              LEVEL9

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES]);
int loadLevel(char* filename, int array[MAP_HEIGHT][MAP_WIDTH]);

#endif
