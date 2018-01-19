#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#include "map.h"
#include "player.h"
#include "video.h"

SDL_Color black={0,0,0};

SDL_Surface* loadImage(char* filename, SDL_Surface* dest)
{
    SDL_Surface* optimized = NULL;
    SDL_Surface* loaded = IMG_Load(filename);
    if(!loaded)
    {
        fprintf(stderr, "%s", IMG_GetError());
        return NULL;
    }

    optimized = SDL_ConvertSurface(loaded, dest->format, 0);
    SDL_FreeSurface(loaded);
    return optimized;
}

void blitImage(SDL_Surface* image, SDL_Surface* dest, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    SDL_BlitSurface(image, NULL, dest, &destRect);
}

void blitTile(SDL_Surface* image, SDL_Rect tileRect, SDL_Surface* dest, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    SDL_BlitSurface(image, &tileRect, dest, &destRect);
}


void drawLevel(int array[MAP_HEIGHT][MAP_WIDTH])
{
    int x, y;

    for(y = 0; y < MAP_HEIGHT; y++)
    {
        for(x = 0; x < MAP_WIDTH; x++)
        {
            blitTile(palette, tile[array[y][x]], screen, x * 32, y * 32);
        }
    }
}

void drawEntities()
{
    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        switch(entities[i].type)
        {
            case ITEM0:
                if (player.inventory[0] == 0)
                    blitTile(items, item[0], screen,
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT);
                break;
            case ITEM1:
                if (player.inventory[1] == 0)
                    blitTile(items, item[1], screen,
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT);
                break;
            case NPC0:
                blitTile(chars, character[1], screen,
                        entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT);

            default:
                break;
        }
    }
}

void drawPlayer(int x, int y)
{
    blitTile(chars, character[0], screen, x, y);
}

void drawText(char *text, int x, int y, SDL_Color fg)
{
    SDL_Surface* textSurface;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = SCREEN_WIDTH;  // can't be bigger than the screen width
    dest.h = SCREEN_HEIGHT; // same as above but with height

    textSurface = TTF_RenderText_Solid(font, text, fg);
    SDL_BlitSurface(textSurface, NULL, screen, &dest);
}

void drawInventory(int x, int y, int xinterval)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (player.inventory[i] == 1)
        {
            blitTile(items, item[i], screen, x, y);
            x += xinterval;
        }
    }
}
