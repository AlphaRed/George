#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "player.h"
#include "video.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;
SDL_Texture* palette;
SDL_Texture* bgOutdoor;
SDL_Texture* bgIndoor;
SDL_Texture* items;
SDL_Texture* chars;
SDL_Texture* cursor;

TTF_Font* font;
SDL_Color black={0,0,0};
SDL_Color white={255,255,255};

SDL_Rect tile[MAX_TILES];
SDL_Rect item[MAX_ITEMS];
SDL_Rect character[MAX_TILES];

int entityanimframe = 0;
int cursorY = 100;


void blitTile(SDL_Texture* texture, SDL_Rect tileRect, int x, int y, int flip)
{
    SDL_Rect destRect;
    destRect.x = SCREEN_SCALE * x;
    destRect.y = SCREEN_SCALE * y;
    destRect.w = SCREEN_SCALE * TILE_WIDTH;
    destRect.h = SCREEN_SCALE * TILE_HEIGHT;

    SDL_RenderCopyEx(renderer, texture, &tileRect, &destRect, 0, 0, flip);
}

void drawBG(Level l)
{
    switch(l)
    {
        case LEVEL2:
        case LEVEL5:
        case LEVEL6:
            SDL_RenderCopy(renderer, bgIndoor, NULL, NULL);
            break;
        default:
            SDL_RenderCopy(renderer, bgOutdoor, NULL, NULL);
            break;
    }

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
            case ITEM_O2:
                if (player.inventory[0] == 0)
                    blitTile(items, item[0],
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM_SNORKEL:
                if (player.inventory[1] == 0)
                    blitTile(items, item[1],
                            entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM_BROCHURE:
                if (player.inventory[2] == 0)
                    blitTile(items, item[2],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM_WINDMILL:
                if (player.inventory[3] == 0)
                    blitTile(items, item[3],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM_MATTRESS:
                if (player.inventory[4] == 0)
                    blitTile(items, item[4],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case ITEM_ACCORDION:
                if (player.inventory[5] == 0)
                    blitTile(items, item[5],
                             entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;

            // NPCs
            case NPC_SCIENTIST:
                blitTile(chars, character[16+entityanimframe],
                        entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_WELDER:
                blitTile(chars, character[18+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_PILOT:
                blitTile(chars, character[20+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_HAIRDRESSER:
                blitTile(chars, character[22+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_DIVER:
                blitTile(chars, character[24+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_WINDMILLOP:
                blitTile(chars, character[26+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_VENDOR:
                blitTile(chars, character[28+entityanimframe],
                         entities[i].x*TILE_WIDTH, entities[i].y*TILE_HEIGHT, SDL_FLIP_NONE);
                break;
            case NPC_TRASHMONSTER:
                blitTile(chars, character[30+entityanimframe],
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
    SDL_DestroyTexture(textTexture);
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

    // check for empty strings as they cause issues
    if (text[0] == '\0' || text2[0] == '\0')
    {
        fprintf(stderr, "textBox(): Empty or NULL string.\n");
        return;
    }

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
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
}

void titleBar(char *text, int width)
{
    SDL_Surface *textSurface;
    SDL_Rect dest;
    dest.x = 10;
    dest.w = width;
    dest.h = FONT_SIZE + 2*SCREEN_SCALE;
    dest.y = 10;

    // check for empty strings as they cause issues
    if (text[0] == '\0')
    {
        fprintf(stderr, "textBox(): Empty or NULL string.\n");
        return;
    }

    // Make the box
    SDL_Color c = {30, 30, 30, 255};
    fillRect(dest, c, 2);

    textSurface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    dest.x += 8; // text margin
    dest.y += 6; //
    dest.w = textSurface->w;
    dest.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &dest);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
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
        // No one
        case 0:
            break;

        // Scientist
        case 1:
            if (player.quest[0] == 2)
                textBox("Alright, this snorkel will do for the machine!", "I'll add it with what we have already.");
            else if (player.quest[1] == 2)
                textBox("Alright, this windmill blade will do for the machine!", "I'll  add it with what we have already.");
            else if (player.quest[3] == 3)
                textBox("Alright, this accordion will do for the machine!", " I'll add it with what we have already.");
            else
                textBox("We need to fix the generator to provide power.", "Igor, fetch me parts to fix it!");
            break;

        // Welder
        case 2:
            if (player.quest[0] >= 1)
                textBox("Get outta my sight.","I already gave you that free oxygen tank!");
            else
                textBox("We fix cars here, not generators...", "But I do have an extra oxygen tank you can have.");
            break;

        // Pilot
        case 3:
            textBox("Dang it. This darn glider can't even fly right...", " ");
            break;

        // Hairdresser
        case 4:
            textBox("I am very busy right now... Why don't you", "take a seat and read a magazine?");
            break;

        // Diver
        case 5:
            if (player.quest[0] == 0)
                textBox("I'd really like to go for a swim,", "but I haven't got an oxygen tank.");
            else if (player.quest[0] == 1)
                textBox("Thanks for the oxygen tank!","Here's a snorkel if you want to join me!");
            else if (player.quest[0] == 2)
                textBox("Thanks again for the oxygen tank!"," ");
            else pstate.talking = 0;
            break;

        // Windmill Operator
        case 6:
            if (player.quest[1] == 0)
                textBox("This windmill doesn't generate nearly ", "enough electricity...");
            else if (player.quest[1] == 1)
                textBox("Oh?... A brochure on geothermal energy?", "I guess I won't be needing this windmill!");
            else if (player.quest[1] == 2)
                textBox("Thanks again for the brochure!", " ");
            else pstate.talking = 0;
            break;

        // Street Vendor
        case 7:
            if (player.quest[3] == 0 || player.quest[3] == 1)
                textBox("Whadda ya want kid? ", "Tell your friends about our fresh mangos!");
            else if (player.quest[3] == 2)
                textBox("Wow, those monsters bought me out.", "Here's something in return for the help.");
            else if (player.quest[3] >= 3)
                textBox("Afraid I'm all sold out because of those monsters.", "Thanks again.");
            else pstate.talking = 0;
            break;

        // Trash Monster
        case 8:
            if (player.quest[3] == 0 || player.quest[3] > 1)
                textBox("GABARAE GAR GAERG", "!TRASH YUM YUM!");
            else if (player.quest[3] == 1)
                textBox("MANGOS? !GAERBARG! WE SHALL PURCHASE MANY", "TELL THIS VENDOR OF YOURS TO DELIVER");
            else pstate.talking = 0;
            break;

        default:
            break;
        }
}

void drawLvlName(int n)
{
    switch(n) // tweak the widths
    {
        case 0:
            titleBar("Outside castle", 260);
            break;
        case 1:
            titleBar("Generator room", 310);
            break;
        case 2:
            titleBar("Town street", 240);
            break;
        case 3:
            titleBar("Windmill", 160);
            break;
        case 4:
            titleBar("Mechanic's shop", 300);
            break;
        case 5:
            titleBar("Barbershop", 220);
            break;
        case 6:
            titleBar("Cliff", 90);
            break;
        case 7:
            titleBar("Pond", 100);
            break;
        case 8:
            titleBar("Dump", 120);
            break;
        default:
            titleBar("Somewhere out there", 410);
            break;
    }
}

void drawCursor(int x, int y)
{
    SDL_Rect r;
    r.w = TILE_WIDTH;
    r.h = TILE_HEIGHT;
    r.x = x;
    r.y = y;
    //SDL_RenderCopy(renderer, cursor, NULL, NULL);
    blitTile(cursor, r, 0, 0, 0);
}
