#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

#include "map.h"
#include "physics.h"
#include "player.h"
#include "video.h"

#define DEBUG

int initSDL()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
       printf("SDL failed to initialize: %s\n", SDL_GetError());
       return 1;
    }
    else
    {
        window = SDL_CreateWindow("Biggun", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN);
        if(window == NULL)
        {
            printf("Window failed to be created: %s\n", SDL_GetError());
            return 1;
        }
        screen = SDL_GetWindowSurface(window);

        // Initialize libraries
        if(IMG_Init(IMG_INIT_PNG) < 0)
        {
            printf("SDL_Image library failed to initialize: %s", IMG_GetError());
            return 2;
        }
        if(TTF_Init()==-1)
        {
            printf("SDL_TTF library failed to initialize: %s", TTF_GetError());
            return 3;
        }
    }
    return 0;
}

void cleanup()
{
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


int checkEvents(SDL_Event eve)
{
    int ptx, pty;   // tile the player is mostly on coordinates for use command

    if(eve.type == SDL_QUIT)
        return 0;
    else if((eve.type == SDL_KEYDOWN) && (eve.key.repeat == 0))
    {
        switch(eve.key.keysym.sym)
        {
        case SDLK_w:
            // can't jump when falling and talking
            if ((pstate.falling != 1) && (pstate.talking != 1))
                pstate.jumping = 1;
            break;
        case SDLK_s:
            // exit talking
            if (pstate.talking == 1) {
                pstate.talking = 0;
                break;
            }

            // find tile player is mostly on
            if ((player.x % TILE_WIDTH) > (TILE_WIDTH/2))
                ptx = (player.x / TILE_WIDTH) + 1;
            else
                ptx = (player.x / TILE_WIDTH);
            if ((player.y % TILE_WIDTH) > (TILE_WIDTH/2))
                pty = (player.y / TILE_WIDTH) + 1;
            else
                pty = (player.y / TILE_WIDTH);
            // check if entity resides there
            for (int i = 0; i < MAX_ENTITIES; i++)
            {
                if ((entities[i].x == ptx) && (entities[i].y == pty))
                {
                    // handle entity
                    fprintf(stderr, "Used entity %d of type %d\n",
                            i, entities[i].type);
                    switch (entities[i].type)
                    {
                        case EXIT1:
                            loadLevel(FILE_LVL1, lvl);
                            loadEntities(FILE_ENT1, entities);
                            player.x = 20 * TILE_WIDTH;
                            player.y = 13 * TILE_HEIGHT;
                            CurrLevel = LEVEL1;
                            break;
                        case EXIT2:
                            loadLevel(FILE_LVL2, lvl);
                            loadEntities(FILE_ENT2, entities);
                            player.x = 0 * TILE_WIDTH;
                            player.y = 13 * TILE_HEIGHT;
                            CurrLevel = LEVEL2;
                            break;
                        case NPC0:
                            pstate.talking = 1;
                            pstate.movingLeft = 0;
                            pstate.movingRight = 0;
                            pstate.jumping = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
            if(CurrLevel == LEVEL1) // check current level first
            {
                if(ptx == 6 && pty == 13)
                    player.inventory[0] = 1; // grab the item
            }
            else if(CurrLevel == LEVEL2)
            {
                if(ptx == 8 && pty == 13)
                    player.inventory[1] = 1;
            }
            break;
        case SDLK_a:
            if (pstate.talking != 1) {
                pstate.movingLeft = 1;
                pstate.movingRight = 0;
            }
            break;
        case SDLK_d:
            if (pstate.talking != 1) {
                pstate.movingRight = 1;
                pstate.movingLeft = 0;
            }
            break;
        case SDLK_RETURN:
            return 0;
            break;
        default:
            break;
        }
    }
    else if (eve.type == SDL_KEYUP)
    {
        switch(eve.key.keysym.sym)
        {
        case SDLK_w:
            pstate.jumping = 0;
            pstate.falling = 1;
            break;
        case SDLK_a:
            pstate.movingLeft = 0;
            break;
        case SDLK_d:
            pstate.movingRight = 0;
            break;
        default:
            break;
        }
    }
    return 1;
}

int main(int argc, char* args[])
{
    int quit = 1;
    unsigned int ticksLastFrame = 0;
    SDL_Event eve;

    // debug strings
#ifdef DEBUG
    char playercoords[15];
    char playerstate[30];
    char playerinv[20];
#endif

    if(initSDL() > 0)
        fprintf(stderr, "Failed to initialize\n");


    // setup tile source rectangles
    for (int i=0; i < MAX_TILES; i++)
    {
        tile[i].w = TILE_WIDTH;
        tile[i].h = TILE_HEIGHT;
        tile[i].x = (i % 10) * TILE_WIDTH;
        tile[i].y = (i / 10) * TILE_HEIGHT;
    }
    for (int i=0; i < MAX_ITEMS; i++)
    {
        item[i].w = TILE_WIDTH;
        item[i].h = TILE_HEIGHT;
        item[i].x = (i % 10) * TILE_WIDTH;
        item[i].y = (i / 10) * TILE_HEIGHT;
    }
    for (int i=0; i < MAX_CHARS; i++)
    {
        character[i].w = TILE_WIDTH;
        character[i].h = TILE_HEIGHT;
        character[i].x = (i % 10) * TILE_WIDTH;
        character[i].y = (i / 10) * TILE_HEIGHT;
    }

    // Load some resources and files
    font = TTF_OpenFont(FILE_FONT, FONT_SIZE);
    if (font == NULL)
    {
        fprintf(stderr, "Failed to load font: %s.\n", FILE_FONT);
        return 1;
    }
    palette = loadImage(FILE_TILES, screen);
    if (palette == NULL)
        return 1;
    bg = loadImage(FILE_BG, screen);
    if (bg == NULL)
        return 1;
    items = loadImage(FILE_ITEMS, screen);
    if (items == NULL)
        return 1;
    chars = loadImage(FILE_CHARS, screen);
    if (chars == NULL)
        return 1;
    if (loadLevel(FILE_LVL1, lvl) > 0)
        return 1;
    CurrLevel = LEVEL1;
    if (loadEntities(FILE_ENT1, entities) > 0)
        return 1;


    player.x = 0;
    player.y = 0;
    player.dx = 0;
    player.dy = 0;
    for(int i = 0; i < MAX_ITEMS; i++) // empty the inventory!
        player.inventory[i] = 0;

    pstate.movingLeft = 0;
    pstate.movingRight = 0;
    pstate.falling = 0;
    pstate.jumping = 0;
    pstate.talking = 0;


    // Game Loop
    while(quit)
    {
        // Events
        SDL_PollEvent(&eve);
        quit = checkEvents(eve);

        // Physics
        gravity(&player.x, &player.y);

        if (pstate.movingLeft && player.dx > -MOVE_SPEED_LIMIT)
            player.dx -= PLAYER_MOVE_ACCEL;
        if (pstate.movingRight && player.dx < MOVE_SPEED_LIMIT)
            player.dx += PLAYER_MOVE_ACCEL;
        if (pstate.jumping)
        {
            player.dy = -PLAYER_JUMP_VEL;
            pstate.jumping++;
            if (pstate.jumping >= PLAYER_JUMP_LIMIT)
            {
                pstate.jumping = 0;
                pstate.falling = 1;
            }
        }

        applyVelocity(&player.x, &player.y, player.dx, player.dy);

        // slow down player
        if (player.dx > 0) player.dx -= PLAYER_FRICTION_X;
        if (player.dx < 0) player.dx += PLAYER_FRICTION_X;
        if (player.dy > 0) player.dy -= PLAYER_FRICTION_Y;
        if (player.dy < 0) player.dy += PLAYER_FRICTION_Y;

        // Outer bounds
        if(player.x < 0)
            player.x = 0;
        if(player.x > (SCREEN_WIDTH-TILE_WIDTH))
            player.x = (SCREEN_WIDTH-TILE_WIDTH);
        if(player.y < 0)
            player.y = 0;
        if(player.y > (SCREEN_HEIGHT-TILE_HEIGHT))
        {
            player.y = (SCREEN_HEIGHT-TILE_HEIGHT);
            pstate.falling = 0;
        }

        // Draw
        blitImage(bg, screen, 0, 0);
        drawLevel(lvl);
        drawEntities();
        drawPlayer(player.x, player.y);
        if (pstate.talking)
            textBox("Kawanishi N1KJ Shiden/Violet Lightning");
        drawInventory(200, 8, 64);


        // debug text
#ifdef DEBUG
        sprintf(playercoords, "X: %d, Y: %d", player.x, player.y);
        drawText(playercoords, 0, 0, black);
        sprintf(playerstate, "states: %d,%d,%d,%d,%d",
                pstate.movingLeft, pstate.movingRight,
                pstate.jumping, pstate.falling, pstate.onGround);
        drawText(playerstate, 0, 20, black);
        sprintf(playerinv, "inv: %d%d%d%d%d%d",
                player.inventory[0],
                player.inventory[1],
                player.inventory[2],
                player.inventory[3],
                player.inventory[4],
                player.inventory[5]);
        drawText(playerinv, 150, 0, black);
#endif

        // Update
        while (SDL_GetTicks() < (1000/FPS_LIMIT + ticksLastFrame))
            SDL_Delay(1);
        SDL_UpdateWindowSurface(window);
        ticksLastFrame = SDL_GetTicks();
    }


    cleanup();

    return 0;
}
