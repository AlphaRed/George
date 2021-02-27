#include <SDL.h>
#include <stdio.h>

#include "map.h"

struct entity entities[MAX_ENTITIES];
int lvl[MAP_HEIGHT][MAP_WIDTH];
Level CurrLevel = LEVEL1;

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES])
{
    FILE* f = fopen(filename, "r");
    int i = 0;

    // file can't open
    if (f == NULL)
    {
        fprintf(stderr, "Couldn't open %s.", filename);
        return 1;
    }

    while((i < MAX_ENTITIES) &&
         (fscanf(f, "%d,%d;%d", &entities[i].x, &entities[i].y, (int*)&entities[i].type) != EOF))
    {
        i++;
    }
    fclose(f);

    // fill rest of entities with nothing
    for (i = i; i < MAX_ENTITIES; i++)
    {
        entities[i].x = 0;
        entities[i].y = 0;
        entities[i].type = ZIPPO;
    }

    return 0;
}

int loadLevel(char* filename, int array[MAP_HEIGHT][MAP_WIDTH])
{
    int x, y;

    FILE* f = fopen(filename, "r");

    // file can't open
    if (f == NULL)
    {
        fprintf(stderr, "Couldn't open %s.", filename);
        return 1;
    }

    for(y = 0; y < MAP_HEIGHT; y++)
    {
        for(x = 0; x < MAP_WIDTH; x++)
        {
            if (fscanf(f, "%d", &array[y][x]) == EOF)
                return 1;
        }
    }
    fclose(f);

    return 0;
}

