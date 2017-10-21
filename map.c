#include <SDL.h>

#include "map.h"

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES])
{
    FILE* f = fopen(filename, "r");
    int i = 0;

    // file can't open
    if (f == NULL)
        return 1;

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

int loadLevel(char* filename, int array[21][21])
{
    int x, y;

    FILE* f = fopen(filename, "r");
    if(f != NULL)
    {
        for(y = 0; y < 21; y++)
        {
            for(x = 0; x < 21; x++)
            {
                fscanf(f, "%d", &array[y][x]);
            }
        }
        fclose(f);
    }
    return 0;
}

