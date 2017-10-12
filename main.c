#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

#include "map.h"
#include "physics.h"
#include "player.h"
#include "video.h"


int initSDL()
{
    int returnflag = 0;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
       printf("SDL failed to initialize: %s\n", SDL_GetError());
       returnflag = 1;
    }
    else
    {
        window = SDL_CreateWindow("Biggun", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN);
        if(window == NULL)
        {
            printf("Window failed to be created: %s\n", SDL_GetError());
            returnflag = 1;
        }
        else
        {
            screen = SDL_GetWindowSurface(window);

            // Initialize libraries
            if(IMG_Init(IMG_INIT_PNG) < 0)
            {
                printf("SDL_Image library failed to initialize: %s", IMG_GetError());
                returnflag = 1;
            }
            if(TTF_Init()==-1)
            {
                printf("SDL_TTF library failed to initialize: %s", TTF_GetError());
                returnflag = 1;
            }
        }
    }
    return returnflag;
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
            // can't jump when falling
            if (pstate.falling != 1)
                pstate.jumping = 1;
            break;
        case SDLK_s:
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
                        case 1:
                            loadLevel("level1.txt", lvl);
                            loadEntities("1.ent", entities);
                            player.x = 20 * TILE_WIDTH;
                            player.y = 13 * TILE_HEIGHT;
                            CurrLevel = Level1;
                            break;
                        case 2:
                            loadLevel("level2.txt", lvl);
                            loadEntities("2.ent", entities);
                            player.x = 0 * TILE_WIDTH;
                            player.y = 13 * TILE_HEIGHT;
                            CurrLevel = Level2;
                            break;
                        default:
                            break;
                    }
                }
            }
            if(CurrLevel == Level1) // check current level first
            {
                if(ptx == 6 && pty == 13)
                    player.inventory[0] = 1; // grab the item
            }
            else if(CurrLevel == Level2)
            {
                if(ptx == 8 && pty == 13)
                    player.inventory[1] = 1;
            }
            break;
        case SDLK_a:
            pstate.movingLeft = 1;
            pstate.movingRight = 0;
            break;
        case SDLK_d:
            pstate.movingRight = 1;
            pstate.movingLeft = 0;
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

    char playercoords[15];
    char playerstate[30];

    if(initSDL() > 0)
        printf("Failed to initialize\n");


    // setup tile source rectangles
    for (int i=0; i < MAX_TILES; i++)
    {
        tile[i].w = TILE_WIDTH;
        tile[i].h = TILE_HEIGHT;
        tile[i].x = (i % 10) * TILE_WIDTH; // should work, but don't have enough tiles to test
        tile[i].y = (i / 10) * TILE_HEIGHT;
    }

    font = TTF_OpenFont(FILE_FONT, FONT_SIZE);
    palette = loadImage(FILE_TILES, screen);
    bg = loadImage("bg.png", screen);
    items = loadImage("items.png", screen);
    loadLevel("level1.txt", lvl);
    CurrLevel = Level1;
    loadEntities("1.ent", entities);


    player.x = 5;
    player.y = 5;
    player.dx = 0;
    player.dy = 0;
    for(int i = 0; i < MAX_ITEMS; i++) // empty the inventory!
        player.inventory[i] = 0;

    pstate.movingLeft = 0;
    pstate.movingRight = 0;
    pstate.falling = 0;
    pstate.jumping = 0;


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
        if(player.x > 640)
            player.x = 640;
        if(player.y < 0)
            player.y = 0;
        if(player.y > 448)
        {
            player.y = 448;
            pstate.falling = 0;
        }

        // Draw
        //SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 0, 255));
        blitImage(bg, screen, 0, 0);
        drawLevel(lvl);
        drawItems();
        drawPlayer(player.x, player.y);


        // debug text
        sprintf(playercoords, "X: %d, Y: %d", player.x, player.y);
        drawText(playercoords, 0, 0, black);
        sprintf(playerstate, "states: %d,%d,%d,%d,%d",
                pstate.movingLeft, pstate.movingRight,
                pstate.jumping, pstate.falling, pstate.onGround);
        drawText(playerstate, 0, 20, black);

        // Update
        while (SDL_GetTicks() < (1000/FPS_LIMIT + ticksLastFrame))
            SDL_Delay(1);
        SDL_UpdateWindowSurface(window);
        ticksLastFrame = SDL_GetTicks();
    }


    cleanup();

    return 0;
}
