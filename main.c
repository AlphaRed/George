#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>

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
        window = SDL_CreateWindow("George", SDL_WINDOWPOS_UNDEFINED,
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
            if (nograv) // nograv mode move up
            {
                pstate.movingUp = 1;
                break;
            }
            // can't jump when falling and talking
            if ((pstate.falling != 1) && (pstate.talking == 0))
                pstate.jumping = 1;
            break;
        case SDLK_s:
            // nograv downwards
            if (nograv) pstate.movingDown = 1;
            break;
        case SDLK_SPACE:
            // exit talking
            if (pstate.talking > 0) {
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
                        case NPC1:
                            pstate.talking = 2;
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
            if (pstate.talking == 0) {
                pstate.movingLeft = 1;
                pstate.movingRight = 0;
            }
            break;
        case SDLK_d:
            if (pstate.talking == 0) {
                pstate.movingRight = 1;
                pstate.movingLeft = 0;
            }
            break;
#ifdef DEBUG
        case SDLK_g: // nograv toggle
            if (nograv) nograv = 0;
            else nograv = 1;
            break;
#endif // DEBUG
        case SDLK_ESCAPE:
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
            if (nograv)
            {
                pstate.movingUp = 0;
                break;
            }
            pstate.jumping = 0;
            pstate.falling = 1;
            break;
        case SDLK_s:
            if (nograv) pstate.movingDown = 0;
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
    int playerframecounter = 0;
    int entityframecounter = 0;
    SDL_Event eve;

    // debug strings
#ifdef DEBUG
    char playercoords[15];
    char playerstate[40];
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
    player.frame = 0;

    pstate.movingLeft = 0;
    pstate.movingRight = 0;
    pstate.falling = 0;
    pstate.jumping = 0;
    pstate.talking = 0;

    nograv = 0;


    // Game Loop
    while(quit)
    {
        // Events
        SDL_PollEvent(&eve);
        quit = checkEvents(eve);

        // Physics
        if (nograv == 0)
            gravity(&player.x, &player.y);

        if (pstate.movingLeft && player.dx > -MOVE_SPEED_LIMIT)
            player.dx -= PLAYER_MOVE_ACCEL;
        if (pstate.movingRight && player.dx < MOVE_SPEED_LIMIT)
            player.dx += PLAYER_MOVE_ACCEL;
        if (pstate.movingUp && player.dy > -MOVE_SPEED_LIMIT)
            player.dy -= PLAYER_MOVE_ACCEL;
        if (pstate.movingDown && player.dy < MOVE_SPEED_LIMIT)
            player.dy += PLAYER_MOVE_ACCEL;
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

        // Animation
        if ((pstate.falling == 0) && (pstate.jumping == 0) &&
            (pstate.movingLeft || pstate.movingRight))
            playerframecounter++;
        else playerframecounter = 0;
        if (pstate.jumping) player.frame = 2;
        if (pstate.falling) player.frame = 3;

        // Draw
        blitImage(bg, screen, 0, 0);
        drawLevel(lvl);
        drawEntities();
        drawPlayer(player.x, player.y);
        switch (pstate.talking)
        {
        case 0:
            break;
        case 1:
            textBox("Kawanishi N1KJ Shiden/Violet Lightning");
            break;
        case 2:
            textBox("The N1K2-J Shiden-Kai offered a formidable, if demanding aircraft, in limited quantities.");
            break;
        default:
            break;
        }
        drawInventory(200, 8, 64);


        // debug text
#ifdef DEBUG
        sprintf(playercoords, "F:%.2d X: %.3d, Y: %.3d", playerframecounter, player.x, player.y);
        drawText(playercoords, 0, 0, black);
        sprintf(playerstate, " ");
        if (pstate.movingLeft)
            strcat(playerstate, "left ");
        if (pstate.movingRight)
            strcat(playerstate, "right ");
        if (pstate.movingDown)
            strcat(playerstate, "down ");
        if (pstate.movingUp)
            strcat(playerstate, "up ");
        if (pstate.jumping)
            strcat(playerstate, "jumping ");
        if (pstate.falling)
            strcat(playerstate, "falling ");
        if (pstate.talking)
            strcat(playerstate, "talking ");
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
        // frames
        entityframecounter++;
        if (entityframecounter >= 100)
        {
            entityframecounter = 0;
            for (int i=0; i < MAX_ENTITIES; i++)
                entityanimframe = 0;
        }
        if (entityframecounter == 50) entityanimframe = 1;
        if (playerframecounter > 20) playerframecounter = 0;
        if (playerframecounter >= 10) player.frame = 1;
        else player.frame = 0;
    }


    cleanup();

    return 0;
}
