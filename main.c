#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "audio.h"
#include "map.h"
#include "physics.h"
#include "player.h"
#include "video.h"

#define DEBUG
#ifdef DEBUG
int debuginfo;
#endif

int initSDL()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
       printf("SDL failed to initialize: %s\n", SDL_GetError());
       return 1;
    }

    window = SDL_CreateWindow("George", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        printf("Window failed to be created: %s\n", SDL_GetError());
        return 1;
    }
    screen = SDL_GetWindowSurface(window);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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
    // Initialize sound
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        printf("SDL_mixer failed to open audio: %s", Mix_GetError());
        return 1;
    }
    return 0;
}

void cleanup()
{
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
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
            {
                pstate.jumping = 1;
                Mix_PlayChannel(-1, sndjump,0);
            }
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
            ptx = round(player.x);
            pty = round(player.y);
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
                            player.x = 19;
                            player.y = 13;
                            CurrLevel = LEVEL1;
                            break;
                        case EXIT2:
                            loadLevel(FILE_LVL2, lvl);
                            loadEntities(FILE_ENT2, entities);
                            player.x = 0;
                            player.y = 13;
                            CurrLevel = LEVEL2;
                            break;
                        case ITEM0:
                        case ITEM1:
                        case ITEM2:
                        case ITEM3:
                        case ITEM4:
                        case ITEM5:
                            if (player.inventory[entities[i].type - 16] == 0) // 16 is just offset to items in the enum
                            {
                                Mix_PlayChannel(-1, snditemget,0);
                                player.inventory[entities[i].type - 16] = 1;
                            }
                            break;
                        case NPC0:
                        case NPC1:
                        case NPC2:
                        case NPC3:
                        case NPC4:
                        case NPC5:
                        case NPC6:
                            pstate.talking = entities[i].type - 21; // 21 is the offset to npcs in the enum
                            pstate.movingLeft = 0;
                            pstate.movingRight = 0;
                            pstate.jumping = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        case SDLK_a:
            if (pstate.talking == 0) {
                pstate.movingLeft = 1;
                pstate.movingRight = 0;
                pstate.facingLeft = 1;
                pstate.facingRight = 0;
            }
            break;
        case SDLK_d:
            if (pstate.talking == 0) {
                pstate.movingRight = 1;
                pstate.movingLeft = 0;
                pstate.facingRight = 1;
                pstate.facingLeft = 0;
            }
            break;
#ifdef DEBUG
        case SDLK_g: // nograv toggle
            if (nograv) nograv = 0;
            else nograv = 1;
            break;
        case SDLK_r: // reset position to origin
            player.x = 0;
            player.y = 0;
            player.dx = 0;
            player.dy = 0;
            break;
        case SDLK_BACKQUOTE: // toggle debug info
            debuginfo = !debuginfo;
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
    debuginfo = 0;
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
        tile[i].x = (i % 16) * TILE_WIDTH;
        tile[i].y = (i / 16) * TILE_HEIGHT;
    }
    for (int i=0; i < MAX_ITEMS; i++)
    {
        item[i].w = TILE_WIDTH;
        item[i].h = TILE_HEIGHT;
        item[i].x = (i % 16) * TILE_WIDTH;
        item[i].y = (i / 16) * TILE_HEIGHT;
    }
    for (int i=0; i < MAX_CHARS; i++)
    {
        character[i].w = TILE_WIDTH;
        character[i].h = TILE_HEIGHT;
        character[i].x = (i % 16) * TILE_WIDTH;
        character[i].y = (i / 16) * TILE_HEIGHT;
    }

    // Load some resources and files
    font = TTF_OpenFont(FILE_FONT, FONT_SIZE);
    if (font == NULL)
    {
        fprintf(stderr, "Failed to load font: %s.\n", FILE_FONT);
        return 1;
    }
    palette = IMG_LoadTexture(renderer, FILE_TILES);
    if (palette == NULL)
        return 1;
    bg = IMG_LoadTexture(renderer, FILE_BG);
    if (bg == NULL)
        return 1;
    items = IMG_LoadTexture(renderer, FILE_ITEMS);
    if (items == NULL)
        return 1;
    chars = IMG_LoadTexture(renderer, FILE_CHARS);
    if (chars == NULL)
        return 1;
    if (loadLevel(FILE_LVL1, lvl) > 0)
        return 1;
    CurrLevel = LEVEL1;
    if (loadEntities(FILE_ENT1, entities) > 0)
        return 1;

    // Load sounds
    sndjump = Mix_LoadWAV(SOUND_JUMP);
    snditemget = Mix_LoadWAV(SOUND_ITEMGET);
    if (!snditemget && !sndjump)
    {
        printf("Sound files fail to load.%s", SDL_GetError());
        return 1;
    }
    Mix_Volume(-1, MIX_MAX_VOLUME/2);

    player.x = 0.0;
    player.y = 0.0;
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
        if (player.dx > 0)
        {
            player.dx -= PLAYER_FRICTION_X;
            if (player.dx < 0) player.dx = 0;
        }
        if (player.dx < 0)
        {
            player.dx += PLAYER_FRICTION_X;
            if (player.dx > 0) player.dx = 0;
        }
        if (player.dy > 0)
        {
            player.dy -= PLAYER_FRICTION_Y;
            if (player.dy < 0) player.dy = 0;
        }
        if (player.dy < 0)
        {
            player.dy += PLAYER_FRICTION_Y;
            if (player.dy > 0) player.dy = 0;
        }

        // Outer bounds
        if (player.x < 0)
            player.x = 0;
        if (player.x > MAP_WIDTH-1)
            player.x = MAP_WIDTH-1;
        if(player.y < 0)
            player.y = 0;
        if(player.y > (MAP_HEIGHT)+1)
        {
            //player.y = (MAP_HEIGHT)-1;
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
        SDL_RenderCopy(renderer, bg, NULL, NULL);
        drawLevel(lvl);
        drawEntities();
        drawPlayer(player.x, player.y);
        switch (pstate.talking)
        {
        case 0:
            break;
        case 1:
            textBox("Igor, you must help me!", "Please find parts to fix the machine.");
            break;
        case 2:
            textBox("Howdy! I'm a welder.", "Need something fixed?");
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            textBox("pew", " ");
            break;
        default:
            break;
        }
        drawInventory(TILE_WIDTH, 0, TILE_WIDTH);


        // debug text
#ifdef DEBUG
        if (debuginfo)
        {
            sprintf(playercoords, "F:%.2d X: %.3f, Y: %.3f", playerframecounter, player.x, player.y);
            drawText(playercoords, 0, 0, white);
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
            drawText(playerstate, 0, 20, white);
            sprintf(playerinv, "inv: %d%d%d%d%d%d",
                    player.inventory[0],
                    player.inventory[1],
                    player.inventory[2],
                    player.inventory[3],
                    player.inventory[4],
                    player.inventory[5]);
            drawText(playerinv, 150, 0, white);
        }
#endif

        // Update
        while (SDL_GetTicks() < (1000/FPS_LIMIT + ticksLastFrame))
            SDL_Delay(1);
        //SDL_UpdateWindowSurface(window);
        SDL_RenderPresent(renderer);
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
