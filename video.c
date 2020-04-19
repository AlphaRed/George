#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "player.h"
#include "video.h"

SDL_Color black={0,0,0};
SDL_Color white={255,255,255};


void blitTile(SDL_Texture* texture, SDL_Rect tileRect, int x, int y, int flip)
{
    SDL_Rect destRect;
    destRect.x = x*(SCREEN_SCALE);
    destRect.y = y*(SCREEN_SCALE);
    destRect.w = TILE_WIDTH * SCREEN_SCALE;
    destRect.h = TILE_HEIGHT * SCREEN_SCALE;

    SDL_RenderCopyEx(renderer, texture, &tileRect, &destRect, 0, 0, flip);
}


void drawLevel(int array[MAP_HEIGHT][MAP_WIDTH])
{
    int x, y;

    for(y = 0; y < MAP_HEIGHT; y++)
    {
        for(x = 0; x < MAP_WIDTH; x++)
        {
            blitTile(palette, tile[array[y][x]], x * TILE_WIDTH, y * TILE_HEIGHT, SDL_FLIP_NONE);
        }
    }
}

void drawEntities()
{
    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        switch(entities[i].type)
        {
            // Items
            case ITEM0:
                if (player.inventory[0] == 0)
                    blitTile(items, item[0],
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM1:
                if (player.inventory[1] == 0)
                    blitTile(items, item[1],
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM2:
                if (player.inventory[2] == 0)
                    blitTile(items, item[2],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM3:
                if (player.inventory[3] == 0)
                    blitTile(items, item[3],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM4:
                if (player.inventory[4] == 0)
                    blitTile(items, item[4],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM5:
                if (player.inventory[5] == 0)
                    blitTile(items, item[5],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;

            // NPCs
            case NPC0:
                blitTile(chars, character[10+entityanimframe],
                        entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC1:
                blitTile(chars, character[12+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC2:
                blitTile(chars, character[14+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC3:
                blitTile(chars, character[16+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC4:
                blitTile(chars, character[18+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC5:
                blitTile(chars, character[20+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC6:
                blitTile(chars, character[22+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;

            default:
                break;
        }
    }
}

void drawPlayer(float x, float y)
{
    int rx = round(x * TILE_WIDTH);
    int ry = round(y * TILE_HEIGHT);
    int flip = SDL_FLIP_NONE;

    if (pstate.facingLeft)
        flip = SDL_FLIP_HORIZONTAL;

    blitTile(chars, character[player.frame], rx, ry, flip);
}

void drawText(char *text, int x, int y, SDL_Color fg)
{
    SDL_Surface* textSurface;
    int textw, texth;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = SCREEN_WIDTH;  // can't be bigger than the screen width
    dest.h = SCREEN_HEIGHT; // same as above but with height

    textSurface = TTF_RenderText_Solid(font, text, fg);
    dest.w = textSurface->w;
    dest.h = textSurface->h;
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &dest);

    SDL_FreeSurface(textSurface);
}

void drawInventory(int x, int y, int xinterval)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (player.inventory[i] == 1)
        {
            blitTile(items, item[i], x, y, SDL_FLIP_NONE);
            x += xinterval;
        }
    }
}

void textBox(char *text)
{
    Uint32 color = SDL_MapRGB(screen->format, 0, 0, 0);
    SDL_Surface* textSurface;
    SDL_Rect dest;
    dest.x = 50;
    dest.w = SCREEN_WIDTH - 100; // screen width minus a bit
    dest.h = (FONT_SIZE * 3) + 4; // fit three lines of text in the box and a little bit
    dest.y = SCREEN_HEIGHT - dest.h - (FONT_SIZE * 1);

    textSurface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_FillRect(screen, &dest, color);
    SDL_RenderFillRect(renderer, &dest);

    dest.x += 2; // text margin
    dest.y += 2; //
    dest.w = textSurface->w;
    dest.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &dest);

    SDL_FreeSurface(textSurface);
}
