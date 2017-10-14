#include <SDL.h>
#include <SDL_image.h>

#include "map.h"
#include "player.h"
#include "video.h"

SDL_Color black={0,0,0};

SDL_Surface* loadImage(char* filename, SDL_Surface* dest)
{
    SDL_Surface* optimized = NULL;
    SDL_Surface* loaded = IMG_Load(filename);
    if(!loaded)
        printf("Image failed to load: %s", IMG_GetError());
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


void drawLevel(int array[21][21])
{
    int x, y;

    for(y = 0; y < 15; y++)
    {
        for(x = 0; x < 21; x++)
        {
            blitTile(palette, tile[array[y][x]], screen, x * 32, y * 32);
        }
    }
}

void drawItems()
{
    if(CurrLevel == LEVEL1) // find current level!
    {
        if(player.inventory[0] == 0) // check if in inventory or not!
            blitImage(items, screen, 6 * TILE_WIDTH, 13 * TILE_HEIGHT);
    }
    else if(CurrLevel == LEVEL2)
    {
        if(player.inventory[1] == 0)
            blitImage(items, screen, 8 * TILE_WIDTH, 13 * TILE_HEIGHT);
    }
}

void drawEntities()
{
    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        switch(entities[i].type)
        {
            case ITEM1:
                if (player.inventory[0] == 0)
                    blitImage(items, screen, 6*TILE_WIDTH, 13*TILE_HEIGHT);
                break;
            case ITEM2:
                if (player.inventory[1] == 0)
                    blitImage(items, screen, 8*TILE_WIDTH, 13*TILE_HEIGHT);
                break;

            default:
                break;
        }
    }
}

void drawPlayer(int x, int y)
{
    blitTile(palette, tile[6], screen, x, y);
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

