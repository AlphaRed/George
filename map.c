#include <SDL.h>

#include "map.h"

int loadEntities(char* filename, struct entity entities[MAX_ENTITIES])
{
    FILE* f = fopen(filename, "r");
    int i = 0;

    while((f != NULL) && (i < MAX_ENTITIES)) // file ends or too many entities
    {
        fscanf(f, "%d,%d;%d", &entities[i].x, &entities[i].y, &entities[i].type);
        i++;
    }
    fclose(f);

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

