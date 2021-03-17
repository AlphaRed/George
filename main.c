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

typedef enum{
    MENU,
    LEVEL,
    WIN
} gameState;

gameState game;

struct protagonist player;
struct states pstate;

int musicenabled;
Mix_Chunk* sndjump;
Mix_Chunk* snditemget;
Mix_Music* musmusic;

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

// Move event functions to new file, no need to clog up main.c
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
            if (pstate.nograv) // nograv mode move up
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
            if (pstate.nograv) pstate.movingDown = 1;
            break;
        case SDLK_SPACE:
            // exit talking
            if (pstate.talking > 0) {
                // 'finished talking with' events
                // Scientist: give snorkel
                if (pstate.talking == 1 && player.quest[0] == 2)
                {
                    player.inventory[1] = 2;
                    player.quest[0]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Scientist: give windmill
                else if (pstate.talking == 1 && player.quest[1] == 2)
                {
                    player.inventory[3] = 2;    // used windmill
                    player.quest[1]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Scientist: give mattress
                else if (pstate.talking == 1 && player.quest[2] == 2)
                {
                    player.inventory[4] = 2;    // used mattress
                    player.quest[2]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Scientist: give accordion
                else if (pstate.talking == 1 && player.quest[3] == 3)
                {
                    player.inventory[5] = 2;   // used accordion
                    player.quest[3]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Diver: O2 tank use, get snorkel event
                else if (pstate.talking == 5 && player.quest[0] == 1)
                {
                    player.inventory[0] = 2;   // used o2 tank
                    player.inventory[1] = 1;   // get snorkel
                    player.quest[0]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Windmill operator: given brochure event
                else if (pstate.talking == 6 && player.quest[1] == 1)
                {
                    player.inventory[2] = 2; // used brochure
                    player.inventory[3] = 1; // got windmill
                    player.quest[1]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Street Vendor: tells about mangos
                else if (pstate.talking == 7 && player.quest[3] == 0)
                    player.quest[3]++;
                // Street Vendor: gives accordion as thanks
                else if (pstate.talking == 7 && player.quest[3] == 2)
                {
                    player.inventory[5] = 1;    // get accordion
                    player.quest[3]++;
                    Mix_PlayChannel(-1, snditemget, 0);
                }
                // Monster: will buy mangos
                else if (pstate.talking == 8 && player.quest[3] == 1)
                    player.quest[3]++;
                // After talking to pilot, he will appear in dump
                else if (pstate.talking == 3 && player.quest[2] == 0 && player.location == LEVEL7)
                    player.quest[2]++;
                // Talk to him in dump with the mattress
                else if(pstate.talking == 3 && player.quest[2] == 1 && player.location == LEVEL9)
                    player.quest[2]++;

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
                        case EXIT1: // To outside castle from machine room
                            loadLevel(FILE_LVL1, lvl);
                            loadEntities(FILE_ENT1, entities);
                            player.x = 12;
                            player.y = 10;
                            player.location = LEVEL1;
                            break;
                        case EXIT2: // To machine room from outside castle
                            loadLevel(FILE_LVL2, lvl);
                            loadEntities(FILE_ENT2, entities);
                            player.x = 0;
                            player.y = 10;
                            player.location = LEVEL2;
                            break;
                        case EXIT3: // To street from outside castle
                            loadLevel(FILE_LVL3, lvl);
                            loadEntities(FILE_ENT3, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL3;
                            break;
                        case EXIT4: // To outside castle from street
                            loadLevel(FILE_LVL1, lvl);
                            loadEntities(FILE_ENT1, entities);
                            player.x = 19;
                            player.y = 12;
                            player.location = LEVEL1;
                            break;
                        case EXIT5: // To windmill from street
                            loadLevel(FILE_LVL4, lvl);
                            loadEntities(FILE_ENT4, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL4;
                            break;
                        case EXIT6: // To street from windmill
                            loadLevel(FILE_LVL3, lvl);
                            loadEntities(FILE_ENT3, entities);
                            player.x = 19;
                            player.y = 11;
                            player.location = LEVEL3;
                            break;
                        case EXIT7: // To repair shop from street
                            loadLevel(FILE_LVL5, lvl);
                            loadEntities(FILE_ENT5, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL5;
                            break;
                        case EXIT8: // To street from repair shop
                            loadLevel(FILE_LVL3, lvl);
                            loadEntities(FILE_ENT3, entities);
                            player.x = 5;
                            player.y = 11;
                            player.location = LEVEL3;
                            break;
                        case EXIT9: // To hairdressers from street
                            loadLevel(FILE_LVL6, lvl);
                            loadEntities(FILE_ENT6, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL6;
                            break;
                        case EXIT10:    // To street from hairdressers
                            loadLevel(FILE_LVL3, lvl);
                            loadEntities(FILE_ENT3, entities);
                            player.x = 14;
                            player.y = 11;
                            player.location = LEVEL3;
                            break;
                        case EXIT11:    // To cliff from windmill
                            loadLevel(FILE_LVL7, lvl);
                            loadEntities(FILE_ENT7, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL7;
                            break;
                        case EXIT12:    // To windmill from cliff
                            loadLevel(FILE_LVL4, lvl);
                            loadEntities(FILE_ENT4, entities);
                            player.x = 19;
                            player.y = 11;
                            player.location = LEVEL4;
                            break;
                        case EXIT13:    // To pond from cliff
                            loadLevel(FILE_LVL8, lvl);
                            loadEntities(FILE_ENT8, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL8;
                            break;
                        case EXIT14:    // To cliff from pond
                            loadLevel(FILE_LVL7, lvl);
                            loadEntities(FILE_ENT7, entities);
                            player.x = 19;
                            player.y = 11;
                            player.location = LEVEL7;
                            break;
                        case EXIT15:    // To dump from pond
                            loadLevel(FILE_LVL9, lvl);
                            if(player.quest[2] == 1)
                                loadEntities(FILE_ENT9B, entities);
                            else
                                loadEntities(FILE_ENT9, entities);
                            player.x = 0;
                            player.y = 11;
                            player.location = LEVEL9;
                            break;
                        case EXIT16:    // To pond from dump
                            loadLevel(FILE_LVL8, lvl);
                            loadEntities(FILE_ENT8, entities);
                            player.x = 19;
                            player.y = 11;
                            player.location = LEVEL8;
                            break;
                        case ITEM0:
                        case ITEM1:
                        case ITEM2:
                        case ITEM3:
                        case ITEM4:
                        case ITEM5:
                            if (player.inventory[entities[i].type - 17] == 0) // 16 is offset to items in enum
                            {
                                Mix_PlayChannel(-1, snditemget,0);
                                player.inventory[entities[i].type - 17] = 1;
                            }

                            // got oxygen tank, quest 0 advance
                            if (player.inventory[0] && entities[i].type == 17 && player.quest[0] == 0)
                                player.quest[0]++;
                            // got brochure, quest 1 advance
                            if (player.inventory[2] && entities[i].type == 19 && player.quest[1] == 0)
                                player.quest[1]++;
                            // got mattress, quest 2 advance
                            if (player.inventory[4] && entities[i].type == 21 && player.quest[2] == 0)
                                player.quest[2]++;
                            break;
                        case NPC0:
                        case NPC1:
                        case NPC2:
                        case NPC3:
                        case NPC4:
                        case NPC5:
                        case NPC6:
                        case NPC7:
                            pstate.talking = entities[i].type - 22; // 21 is offset to NPCs in enum
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
        case SDLK_m:
            musicenabled = !musicenabled;
            if (musicenabled) Mix_VolumeMusic(MIX_MAX_VOLUME/4);
            else Mix_VolumeMusic(0);
            break;
#ifdef DEBUG
        case SDLK_g: // nograv toggle
            if (pstate.nograv) pstate.nograv = 0;
            else pstate.nograv = 1;
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
        case SDLK_p:
            game = WIN;
            break;
#endif // DEBUG
        case SDLK_ESCAPE:
        case SDLK_RETURN:
            game = MENU;
#ifdef DEBUG
            debuginfo = 0;
#endif // DEBUG
            Mix_FadeOutMusic(MUSIC_FADE);
            Mix_HaltMusic();
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
            if (pstate.nograv)
            {
                pstate.movingUp = 0;
                break;
            }
            pstate.jumping = 0;
            pstate.falling = 1;
            break;
        case SDLK_s:
            if (pstate.nograv) pstate.movingDown = 0;
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

int checkMenu(SDL_Event eve)
{
    int mouseX, mouseY;

    if(eve.type == SDL_QUIT)
        return 0;
    /*else if((eve.type == SDL_KEYDOWN) && (eve.key.repeat == 0))
    {
        switch(eve.key.keysym.sym)
        {
            case SDLK_UP:
                cursorY = 50;
                break;
        }
    }*/
    else if(eve.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_GetMouseState(&mouseX, &mouseY);

        // "Start"
        if(mouseX >= (3 * TILE_WIDTH * SCREEN_SCALE) && mouseX <= (6 * TILE_WIDTH * SCREEN_SCALE))
        {
            if(mouseY >= (4 * TILE_HEIGHT * SCREEN_SCALE) && mouseY <= (5 * TILE_HEIGHT * SCREEN_SCALE))
            {
                game = LEVEL;
                // reset player
                player.x = 0.0;
                player.y = 0.0;
                player.dx = 0;
                player.dy = 0;
                for(int i = 0; i < MAX_ITEMS; i++) // empty the inventory!
                player.inventory[i] = 0;
                player.frame = 0;
                player.quest[0] = 0;    // for snorkel
                player.quest[1] = 0;    // for windmill
                player.quest[2] = 0;    // for mattress
                player.quest[3] = 0;    // for accordion
                player.location = LEVEL1;
                // play music
                if (Mix_FadeInMusic(musmusic, -1, MUSIC_FADE) == -1)
                    printf("Mix ERROR: %s", Mix_GetError());
                // load first level
                if (loadLevel(FILE_LVL1, lvl) > 0)
                    return 0;   // something went wrong!
                player.location = LEVEL1;
                if (loadEntities(FILE_ENT1, entities) > 0)
                    return 0;   // something went wrong!
                return 1;
            }
        }
        // "Exit"
        if(mouseX >= (3 * TILE_WIDTH * SCREEN_SCALE) && mouseX <= (5 * TILE_WIDTH * SCREEN_SCALE))
        {
            if(mouseY >= (6 * TILE_HEIGHT * SCREEN_SCALE) && mouseY <= (7 * TILE_HEIGHT * SCREEN_SCALE))
            {
                // Quit game
                return 0;
            }
        }

    }
    return 1;
}

int checkWin(SDL_Event eve)
{
    if(eve.type == SDL_QUIT)
        return 0;
    else if(eve.type == SDL_MOUSEBUTTONDOWN)
    {
        game = MENU;
        return 1;
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
    setupTile(tile, MAX_TILES);
    setupTile(item, MAX_ITEMS);
    setupTile(character, MAX_CHARS);

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
    bgOutdoor = IMG_LoadTexture(renderer, FILE_BG_OUTDOOR);
    if (bgOutdoor == NULL)
        return 1;
    bgIndoor = IMG_LoadTexture(renderer, FILE_BG_INDOOR);
    if(bgIndoor == NULL)
        return 1;
    items = IMG_LoadTexture(renderer, FILE_ITEMS);
    if (items == NULL)
        return 1;
    chars = IMG_LoadTexture(renderer, FILE_CHARS);
    if (chars == NULL)
        return 1;
    if (loadLevel(FILE_LVL1, lvl) > 0)
        return 1;
    if (loadEntities(FILE_ENT1, entities) > 0)
        return 1;

    SDL_Texture* mainMenu = IMG_LoadTexture(renderer, "sprites/menu.png");
    if(mainMenu == NULL)
        return 1;
    cursor = IMG_LoadTexture(renderer, "sprites/cursor.png");
    if(cursor == NULL)
        return 1;

    // Load sounds
    sndjump = Mix_LoadWAV(SOUND_JUMP);
    snditemget = Mix_LoadWAV(SOUND_ITEMGET);
    musmusic = Mix_LoadMUS(MUSIC_1);
    if (!snditemget && !sndjump && !musmusic)
    {
        printf("Sound files fail to load.%s", Mix_GetError());
        return 1;
    }
    Mix_Volume(-1, MIX_MAX_VOLUME/2);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    musicenabled = 1;

    player.x = 0.0;
    player.y = 0.0;
    player.dx = 0;
    player.dy = 0;
    for(int i = 0; i < MAX_ITEMS; i++) // empty the inventory!
        player.inventory[i] = 0;
    player.frame = 0;
    player.quest[0] = 0;    // for snorkel
    player.quest[1] = 0;    // for windmill
    player.quest[2] = 0;    // for mattress
    player.quest[3] = 0;    // for accordion
    player.location = LEVEL1;

    pstate.movingLeft = 0;
    pstate.movingRight = 0;
    pstate.falling = 0;
    pstate.jumping = 0;
    pstate.talking = 0;

    pstate.nograv = 0;

    game = MENU;

    // Game Loop
    while(quit)
    {
        // Events
        SDL_PollEvent(&eve);
        if(game == LEVEL)
        {
            quit = checkEvents(eve);
        }
        else if(game == MENU)
        {
            quit = checkMenu(eve);
        }
        else if(game == WIN)
        {
            quit = checkWin(eve);
        }

        // Physics
        // should move this to the physics c file
        if (pstate.nograv == 0)
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

        applyVelocity(&player.x, &player.y, &player.dx, &player.dy);

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
            game = MENU; // kill him
            Mix_PlayChannel(0, snditemget, 0);
            Mix_FadeOutMusic(MUSIC_FADE);
        }
        // Check win
        if(player.quest[0] == 3 && player.quest[1] == 3 &&
           player.quest[2] == 3 && player.quest[3] == 4)
            game = WIN;

        // Animation
        if ((pstate.falling == 0) && (pstate.jumping == 0) &&
            (pstate.movingLeft || pstate.movingRight))
            playerframecounter++;
        else playerframecounter = 0;
        if (pstate.jumping) player.frame = 2;
        if (pstate.falling) player.frame = 3;

        // Draw
        if(game == LEVEL)
        {
            drawBG(player.location);
            drawLevel(lvl);
            drawEntities();
            drawPlayer(player.x, player.y);
            drawTextBox(pstate.talking);
            drawInventory(TILE_WIDTH, 25, TILE_WIDTH);
            drawLvlName(player.location);
        }
        else if(game == MENU)
        {
            SDL_RenderCopy(renderer, mainMenu, NULL, NULL);
            //drawCursor(cursorX, cursorY);
        }
        else if(game == WIN)
        {
            SDL_RenderCopy(renderer, bgIndoor, NULL, NULL);
            drawText("You Win!", 50, 100, white);
            drawText("Thanks for playing!", 50, 120, white);
        }

        // debug text
#ifdef DEBUG
        if (debuginfo)
        {
            sprintf(playercoords, "F:%.2d X: %.3f, Y: %.3f  dx:%.3f  dy:%.3f", playerframecounter, player.x, player.y, player.dx, player.dy);
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
            drawText(playerinv, 150, 20, white);
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
