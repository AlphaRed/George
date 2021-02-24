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
    destRect.x = SCREEN_SCALE * x;
    destRect.y = SCREEN_SCALE * y;
    destRect.w = SCREEN_SCALE * TILE_WIDTH;
    destRect.h = SCREEN_SCALE * TILE_HEIGHT;

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
                blitTile(chars, character[16+entityanimframe],
                        entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC1:
                blitTile(chars, character[18+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC2:
                blitTile(chars, character[20+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC3:
                blitTile(chars, character[22+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC4:
                blitTile(chars, character[24+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC5:
                blitTile(chars, character[26+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC6:
                blitTile(chars, character[28+entityanimframe],
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
    SDL_Rect dest;
    dest.x = SCREEN_SCALE * x;
    dest.y = SCREEN_SCALE * y;
    dest.w = SCREEN_SCALE * SCREEN_WIDTH;  // can't be bigger than the screen width
    dest.h = SCREEN_SCALE * SCREEN_HEIGHT; // same as above but with height

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

void fillRect(SDL_Rect rect, SDL_Color color, int border)
{
    SDL_Color borderColor = {255, 255, 255, 255}; // Borders are always white for now
    if(border > 0)
    {
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderFillRect(renderer, &rect);

        rect.x += border;
        rect.y += border;
        rect.w -= border * 2;
        rect.h -= border * 2;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void textBox(char *text, char *text2)
{
    SDL_Surface *textSurface;
    SDL_Surface *textSurface2;
    SDL_Rect dest;
    dest.x = 0;
    dest.w = SCREEN_WIDTH;
    dest.h = (FONT_SIZE * 2) + 4*SCREEN_SCALE; // fit three lines of text in the box and a little bit
    dest.y = SCREEN_HEIGHT - dest.h - (FONT_SIZE * 1);

    // Make the box
    SDL_Color c = {30, 30, 30, 255};
    fillRect(dest, c, 2);

    textSurface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textSurface2 = TTF_RenderText_Solid(font, text2, white);
    SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);

    dest.x += 6; // text margin
    dest.y += 6; //
    dest.w = textSurface->w;
    dest.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &dest);
    dest.y += 2 + FONT_SIZE; // for second line
    dest.w = textSurface2->w;
    dest.h = textSurface2->h;
    SDL_RenderCopy(renderer, textTexture2, NULL, &dest);

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(textSurface2);
}

void setupTile(SDL_Rect t[], int num)
{
    for (int i=0; i < num; i++)
    {
        t[i].w = TILE_WIDTH;
        t[i].h = TILE_HEIGHT;
        t[i].x = (i % 16) * TILE_WIDTH;
        t[i].y = (i / 16) * TILE_HEIGHT;
    }
}

void drawTextBox(int n)
{
    switch (n)
    {
        case 0: // no one
            break;
        case 1: // scientist
            textBox("We need to fix the generator to provide power.", "Igor, fetch me parts to fix it!");
            break;
        case 2: // welder
            textBox("We fix cars here, not generators...", " ");
            break;
        case 3: // pilot
            textBox("Dang it. This darn glider can't even fly right...", " ");
            break;
        case 4: // hairdresser
            textBox("You can't have our fan but I can give you a haircut!", "What will it be then?");
            break;
        case 5: // diver
            textBox("I'd really like to go for a swim,", "but I haven't got an oxygen tank.");
            break;
        case 6: // windmill operator
            textBox("This windmill doesn't generate nearly ", "enough electricity...");
            break;
        case 7: // street vendor
            textBox("Whadda ya want kid? ", "How about a juicy mango?");
            break;
        default:
            break;
        }
}
